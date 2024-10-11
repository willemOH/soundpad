#ifndef PLAYBACK_H
#define PLAYBACK_H

#include "daisy_seed.h"
#include "stereo_pair.h"
#include "sample_settings.h"
#include "stereo_buffer_chunk.h"
#include "global.h"
using namespace daisy;
using namespace daisysp;

class Playback {

private:
    
   
    float factor; // how much to advance index for a new sample
    float freq; // in Hz
    bool play;
    
    bool nill;


public:
    Playback(SampleSettings& setts) : settings(setts){
        index = 0.0f;
        freq = 440.0f;
        factor = (freq / 440.0f);
    }
    float index; // index into buffer
    bool loop;
    uint32_t start;
    uint32_t end;
    StereoBufferChunk* sBuffer;
    void Nothing();
    void ProcessAudioFrame();
    void SetNote(float slider);
    StereoPair output;
    SampleSettings& settings;
    void PlayPrepare();

};

#endif // PLAYBACK_H