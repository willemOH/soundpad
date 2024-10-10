#ifndef SETTINGS_H
#define SETTINGS_H

#include "daisy_seed.h"
using namespace daisy;

struct SampleSettings {
        uint32_t length; // length of sample, < BUFFER_MAX
        uint32_t loopStart;
        uint32_t loopEnd;
        uint32_t startSaved;
        uint32_t endSaved;
        float speed;
        float tune;
};

#endif // SETTINGS_H