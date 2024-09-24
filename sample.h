#pragma once

#include "daisy_seed.h"
#include "stereo_buffer_chunk.h"
#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 

using namespace daisy;

class Sample {
private:
    float sIndex; // index into buffer
    uint32_t sIndexInt;
    float sIndexFraction;
    uint32_t sIndexRecord;
    float sFactor; // how much to advance index for a new sample
    float sFreq; // in Hz
    bool sGate, sGatePrev; //

    StereoBufferChunk* sBuffer;

   


    struct SampleSettings //this would be in a more global class for sampling
{
	// sample
	// sssssssssssssssssssss
	// start lstart lend end
	// gate--<--------->
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

    StereoPair output;

    void Record(float sigL, float sigR); //record functionality will be in separate class
    void Playback();
public:
    void Init(float sampleRate, StereoBufferChunk* soundBuffer);
    SampleSettings settings;
    void Process(float sigL, float sigR, bool record);
    StereoPair GetOutput(){
        return(output);
    }; 
    void RecordPrepare(bool yes);
    void FillBuffer(float sampleRate);


    // Add other member functions and variables here
};
