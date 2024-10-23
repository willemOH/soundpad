#pragma once

#include "daisy_seed.h"
#include "i_sample.h"
#include "stereo_buffer_chunk.h"
#include "sample_settings.h"
#include "stereo_pair.h"
#include "playback.h"
#include "global.h"

#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 

using namespace daisy;

class Sample : public ISample{
private:

    uint32_t indexRecord;
    
    
    bool gate, gatePrev; //

    StereoBufferChunk* sBuffer;

    uint32_t start;
    uint32_t end;
    static uint32_t previewTime; //seconds

    static float scale[8];

    StereoPairRef inputRef;
    Playback playback;
    WavWriter<32768>& sdWriter;
    StereoPair output;
    bool fileOpened = false;
    void Record() override; //record functionality will be in separate class
    

public:
    Sample(float& left, float& right, WavWriter<32768>& writer); 
    void Init(float sampleRate, StereoBufferChunk* soundBuffer);
    SampleSettings settings;
    void Process();
    void ProcessAudioFrame();
    void WriteProcess();
    StereoPair GetOutput();
    bool record;
    bool recording;
    bool play;
    void RecordPrepare() override;
    void PlayPrepare() override;
    void FillBuffer(float sampleRate);
    void SetRecord(bool recordState) override;
    void SetPlayback(bool playState) override;
    void SetLoop(bool loopState) override;
    void SetStart(float fraction) override;
    void SetEnd(float fraction) override;
    bool GetLoopState() override;
    void SetSpeed(float ratio) override;
    void SetNote(float noteNum) override;

    
};
