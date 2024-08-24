#ifndef BUFFERSTEREO_H
#define BUFFERSTEREO_H

#pragma once

#include "daisysp.h"
//using namespace daisy;
using namespace daisysp; 

// cannot define buffers to SDRAM within an object: https://forum.electro-smith.com/t/possible-runtime-crash-trying-to-put-an-object-in-the-sdram/3389/4
class BufferStereo {
private:
    float* bufferL;
    float* bufferR;

    struct StereoPair{
        float left;
        float right;
    };

    StereoPair getValueInterpolated(float index) const{ 
        int32_t indexInt = static_cast<int32_t>(index); //strips decimal
        float indexFraction = index - indexInt; //gets decimal

        StereoPair current = getValue(indexInt);
        StereoPair next = getValue(indexInt + 1);

        // Linear interpolation (value halfway between samples)
        float sigL = current.left + (next.left - current.left) * indexFraction;
        float sigR = current.right + (next.right - current.right) * indexFraction;

        return {sigL, sigR}; 
    }

    StereoPair getValue(uint32_t index) const { //const = readonly
            return {bufferL[index], bufferR[index]};
    } 

public:
    BufferStereo(float* bufL, float* bufR) 
        : bufferL(bufL), bufferR(bufR) {}
    
    void setValue(uint32_t index, float left, float right) {
            bufferL[index] = left;
            bufferR[index] = right;
    }


    StereoPair getSample(uint32_t index) const{
        return {getValueInterpolated(index)};
    }
        

};

#endif // BUFFERSTEREO_H