#include "sample.h"

Sample::Sample(float& left, float& right, WavWriter<32768>& writer) : inputRef(left, right), playback(settings), sdWriter(writer) {} //references must be intialized upon construction

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
    settings.startSaved = 48000.0f * 0.0f;
	settings.loopStart = 48000.0f * 0.0f;
	settings.loopEnd = 48000.0f * 0.5f;
	settings.endSaved = 48000.0f * 1.0f;
	settings.length = settings.endSaved;
	settings.speed = 1;
	settings.tune = 0;

	play = false; //no good. play gets set to true on startup after this
	record = false;

	playback.sBuffer = soundBuffer;
}
void Sample::WriteProcess(){
	if(record){
		recording = true;
		if(!fileOpened){
			sdWriter.OpenFile("sample1.wav");
			fileOpened = true;
		}
		sdWriter.Write();
	}
	else if(recording){
			sdWriter.SaveFile();
            //hardware.PrintLine("file saved"); //audio callback does not like printlines
			recording = false;
			fileOpened = false;
		}
}

void Sample::Process() {
	if(record){
		Record();
		}
	if(play){
		playback.ProcessAudioFrame();
	}
	else{ //TODO: add this signal to playback instead of being replaced by it
		// pass through
		playback.output.right = inputRef.right;
		playback.output.left = inputRef.left;
	}
}

StereoPair Sample::GetOutput(){
        //return output;
        return(playback.output);
    }; 

void Sample::Record(){
	if (indexRecord < (BUFFER_MAX - 1))
	{
		sBuffer->setValue(indexRecord, inputRef.left, inputRef.right);
		float write[2]; 
		write[0] = inputRef.left; 
   	 	write[1] = inputRef.right;    
		sdWriter.Sample(write); 
		indexRecord++;
	}
	settings.loopEnd = indexRecord;
	settings.endSaved = indexRecord;
	end = settings.endSaved;
	settings.length = indexRecord;
}

void Sample::RecordPrepare(){ 
	indexRecord = 0;
	settings.startSaved = 0;
	settings.loopStart = 0;

}

void Sample::SetStart(float fraction){
	settings.startSaved = static_cast<uint32_t>(settings.length * fraction);
	playback.start = settings.startSaved;
	if(!settings.loop){
	playback.end = playback.start + previewTime;
	playback.index = playback.start;
	}
	else{
		playback.loop = true;
	}
	SetPlayback(true);
}

void Sample::SetEnd(float fraction){
	settings.endSaved = static_cast<uint32_t>(settings.length * fraction);
	playback.end = settings.endSaved;
	if(!settings.loop){
	playback.start = playback.end - previewTime;
	playback.index = playback.start;
	}
	else{
		playback.loop = true;
	} 
	SetPlayback(true);
}

void Sample::PlayPrepare(){
	playback.PlayPrepare();
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
	play = playState;
}

void Sample::SetLoop(bool loopState){
	settings.loop = loopState;
}

bool Sample::GetLoopState(){
	return settings.loop;
}

void Sample::SetSpeed(float ratio){
	settings.speed = ratio * 2;
	if(!settings.loop){
	playback.PlayPrepare();
	SetPlayback(true);
	}
	else{
	SetPlayback(true);
	}
}

void Sample::SetNote(float noteNum){
	playback.SetNote(noteNum);
}

