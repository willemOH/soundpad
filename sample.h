#pragma once

#include "daisy_seed.h"
#include "i_sample.h"
#include "stereo_buffer_chunk.h"
#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 

using namespace daisy;

class Sample : public ISample{
private:
    float sIndex; // index into buffer
    uint32_t sIndexInt;
    float sIndexFraction;
    uint32_t sIndexRecord;
    float sFactor; // how much to advance index for a new sample
    float sFreq; // in Hz
    bool sGate, sGatePrev; //

    StereoBufferChunk* sBuffer;

    struct SampleSettings{ //this would be in a more global class for sampling
        uint32_t sLength; // length of sample, < BUFFER_MAX
        uint32_t sPhaseStart;
        uint32_t sPhaseLoopStart;
        uint32_t sPhaseLoopEnd;
        uint32_t sPhaseEnd;
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
    void Record() override; //record functionality will be in separate class
    void Playback() override;
    void SetStart(float fraction);
    void SetEnd(float fraction);

    
};
