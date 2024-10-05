#include "sample.h"
#include "global.h"

Sample::Sample(float& left, float& right) : inputRef(left, right) {} //references must be intialized upon construction

void Sample::Init(float sampleRate, StereoBufferChunk* soundBuffer){
    sBuffer = soundBuffer;
    FillBuffer(sampleRate);

	index = 0.0f;
	freq = 440.0f;
	factor = (freq / 440.0f);
    settings.start = 48000.0f * 0.0f;
	settings.loopStart = 48000.0f * 0.0f;
	settings.loopEnd = 48000.0f * 0.5f;
	settings.end = 48000.0f * 1.0f;
	settings.length = settings.end;
}

void Sample::Process() {
	if(record){
			Record();
		}
	if(playback){
		Playback();
	}
	else{
		// pass through
	Sample::output.right = inputRef.right;
	Sample::output.left = inputRef.left;
	}
}

void Sample::Record(){
	if (indexRecord < (BUFFER_MAX - 1))
	{
		sBuffer->setValue(indexRecord, inputRef.left, inputRef.right);
		indexRecord++;

		settings.loopEnd = indexRecord;
		settings.end = indexRecord;
		settings.length = indexRecord;
	}
}

void Sample::Playback(){
    if (index < settings.end)
        {
            float sigL = sBuffer->getSample(index).left;
            float sigR = sBuffer->getSample(index).right;
            index += 1.0;
            //index += factor;
			/* if (loop){
				if (index >= settings.loopEnd)
				{
					index = settings.loopStart;
				}
			}; */
            Sample::output.left=sigL, output.right=sigR;
        #ifdef LOGG
        printFlag = true;
        #endif
        }
        else if(loop){
			index = settings.start;
        }
		else{
			Sample::SetPlayback(false);
		}
}

void Sample::RecordPrepare(){ 
	indexRecord = 0;
	settings.start = 0;
	settings.loopStart = 0;
}

void Sample::SetStart(float fraction){
	settings.start = static_cast<uint32_t>(settings.length * fraction);
}

void Sample::SetEnd(float fraction){
	settings.end = static_cast<uint32_t>(settings.length * fraction);
}

void Sample::PlayPrepare(){
	index = settings.start;
}

// demo
void Sample::FillBuffer(float sampleRate){
	Oscillator osc;
    osc.Init(sampleRate);
    osc.SetWaveform(osc.WAVE_TRI);
	osc.SetFreq(440.0f);
	osc.SetAmp(0.1f);

	for (uint32_t i = 0; i < BUFFER_MAX; i++)
	{
		float signal = osc.Process();
		sBuffer->setValue(i, signal, signal);
	}
	/* // Print the size of sBufferL
    size_t sizeOfsBufferL = sizeof(BufferL) / sizeof(BufferL[0]);
	hardware.PrintLine("Size of sBufferL: %u", static_cast<unsigned int>(sizeOfsBufferL)); */
}

void Sample::SetRecord(bool recordState){
	record = recordState;
}

void Sample::SetPlayback(bool playState){
	playback = playState;
	hardware.PrintLine("loop= %d", loop);
}

void Sample::SetLoop(bool loopState){
	loop = loopState;
	hardware.PrintLine("loop= %d", loop);
}

