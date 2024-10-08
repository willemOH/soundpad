#pragma once

#include "daisy_seed.h"
#include "i_sample.h"
#include "stereo_buffer_chunk.h"
#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 

using namespace daisy;

class Sample : public ISample{
private:
    float index; // index into buffer
    uint32_t indexInt;
    float indexFraction;
    uint32_t indexRecord;
    float factor; // how much to advance index for a new sample
    float freq; // in Hz
    bool gate, gatePrev; //

    StereoBufferChunk* sBuffer;

    uint32_t start;
    uint32_t end;
    static uint32_t previewTime; //seconds

    struct SampleSettings{ //this would be in a more global class for sampling
        uint32_t length; // length of sample, < BUFFER_MAX
        uint32_t loopStart;
        uint32_t loopEnd;
        uint32_t startSaved;
        uint32_t endSaved;
        float speed;
        float tune;
    };

    struct StereoPair{
        float left;
        float right;
    };

    struct StereoPairRef{
        float& left;
        float& right;
        StereoPairRef(float& l, float& r) : left(l), right(r) {}
    };

    StereoPairRef inputRef;
    StereoPair output;
    void Record() override; //record functionality will be in separate class
    void Playback() override;
    

public:
    Sample(float& left, float& right); 
    void Init(float sampleRate, StereoBufferChunk* soundBuffer);
    SampleSettings settings;
    void Process();
    StereoPair GetOutput(){
        return(output);
    }; 
    bool record;
    bool playback;
    bool loop;
    void RecordPrepare() override;
    void PlayPrepare() override;
    void FillBuffer(float sampleRate);
    void SetRecord(bool recordState) override;
    void SetPlayback(bool playState) override;
    void SetLoop(bool loopState) override;
    void SetStart(float fraction);
    void SetEnd(float fraction);
    bool GetLoopState();
    void SetSpeed(float ratio);

    
};
