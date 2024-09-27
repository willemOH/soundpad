#include "sample.h"
#include "global.h"

Sample::Sample(float& left, float& right) : inputRef(left, right) {} //references must be intialized upon construction

void Sample::Init(float sampleRate, StereoBufferChunk* soundBuffer){
    sBuffer = soundBuffer;
    FillBuffer(sampleRate);

	sIndex = 0.0f;
	sFreq = 440.0f;
	sFactor = (sFreq / 440.0f);
    settings.sPhaseStart = 48000.0f * 0.0f;
	settings.sPhaseLoopStart = 48000.0f * 0.0f;
	settings.sPhaseLoopEnd = 48000.0f * 0.5f;
	settings.sPhaseEnd = 48000.0f * 1.0f;
	settings.sLength = settings.sPhaseEnd;
}

void Sample::Process() {
	if(record){
			Record();
		}
		if(playback){
			Playback();
		}
}

void Sample::Record(){
	if (sIndexRecord < (BUFFER_MAX - 1))
	{
		sBuffer->setValue(sIndexRecord, inputRef.left, inputRef.right);
		sIndexRecord++;

		settings.sPhaseLoopEnd = sIndexRecord;
		settings.sPhaseEnd = sIndexRecord;
		settings.sLength = sIndexRecord;
	}
	// pass through
	Sample::output.right = inputRef.right;
	Sample::output.left = inputRef.left;
}

void Sample::Playback(){
    if (sIndex < settings.sPhaseEnd)
        {
            float sigL = sBuffer->getSample(sIndex).left;
            float sigR = sBuffer->getSample(sIndex).right;

            sIndex += 1.0;
            //sIndex += sFactor;
            if (sIndex >= settings.sPhaseLoopEnd)
            {
                sIndex = settings.sPhaseLoopStart;
            }
            
            Sample::output.left=sigL, output.right=sigR;
        #ifdef LOGG
        printFlag = true;
        #endif
        }
        else{
			sIndex = 0;
        }
    
}

void Sample::RecordPrepare(){ 
		sIndexRecord = 0;
		settings.sPhaseStart = 0;
		settings.sPhaseLoopStart = 0;
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
}

