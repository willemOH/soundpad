#include "sample.h"
#include "global.h"

Sample::Sample(float& left, float& right) : inputRef(left, right) {} //references must be intialized upon construction

uint32_t Sample::previewTime = 0.1 * 48000; //in seconds times samples

float Sample::scale[8] = {
	0.f,
	2.f,
	4.f,
	5.f,
	7.f,
	9.f,
	11.f,
	12.f,
};

void Sample::Init(float sampleRate, StereoBufferChunk* soundBuffer){
    sBuffer = soundBuffer;
    FillBuffer(sampleRate);

	index = 0.0f;
	freq = 440.0f;
	factor = (freq / 440.0f);
    start = 48000.0f * 0.0f;
	settings.loopStart = 48000.0f * 0.0f;
	settings.loopEnd = 48000.0f * 0.5f;
	end = 48000.0f * 1.0f;
	settings.length = end;
	settings.speed = 1;
	settings.tune = 0;
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
	}
	settings.loopEnd = indexRecord;
	settings.endSaved = indexRecord;
	end = settings.endSaved;
	settings.length = indexRecord;
}

void Sample::Playback(){
    if (index < end)
        {
            float sigL = sBuffer->getSample(index).left;
            float sigR = sBuffer->getSample(index).right;
			//index += 1.0;
      		index += factor;		
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
			index = start;
        }
		else{ //making this else if(!loop) caused weird play button holding glitches
			Sample::SetPlayback(false);
		}
}

void Sample::SetNote(float slider){
	 static auto hasNoteChanged = [](float newNoteNum) { //temp code to hear note change better
        static float lastNoteNum = -1.0f; 
        if (newNoteNum != lastNoteNum) {
            lastNoteNum = newNoteNum;
            return true;
        }
        return false;
    };
	int scaledValue = static_cast<int>(std::floor(slider * 8));
	float noteNum = scaledValue % 8;

	if (hasNoteChanged(noteNum)) {
	freq = daisysp::mtof(noteNum + (12.f * 6));
	if(!loop){
	index = start;
	factor = settings.speed * (freq / 440.0f);
	SetPlayback(true);
	}
	}
}

void Sample::RecordPrepare(){ 
	indexRecord = 0;
	start = 0;
	settings.startSaved = start;
	settings.loopStart = 0;

}

void Sample::SetStart(float fraction){
	start = static_cast<uint32_t>(settings.length * fraction);
	settings.startSaved = start;
	if(!loop){
	index = start;
	end = start + previewTime;
	SetPlayback(true);
	}
}

void Sample::SetEnd(float fraction){
	end = static_cast<uint32_t>(settings.length * fraction);
	settings.endSaved = end;
	if(!loop){
	start = end - previewTime;
	index = start;
	SetPlayback(true);
	}
}

void Sample::PlayPrepare(){
	start = settings.startSaved;
	end = settings.endSaved;
	index = start;
	factor = settings.speed * (freq / 440.0f);
/* 	factor = settings.speed * (freq / 440.0f) * (1.0f + std::pow(2.0f, settings.tune / 1200.0f)); */
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

void Sample::SetLoop(bool loopState){
	loop = loopState;
}

bool Sample::GetLoopState(){
	return loop;
}

void Sample::SetSpeed(float ratio){
	settings.speed = ratio * 2;
	factor = settings.speed;
	if(!loop){
	index = start;
	SetPlayback(true);
	}
}

