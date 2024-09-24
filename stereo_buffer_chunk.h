#pragma once

#include "daisysp.h"
//using namespace daisy;
using namespace daisysp; 

// cannot define buffers to SDRAM within an object: https://forum.electro-smith.com/t/possible-runtime-crash-trying-to-put-an-object-in-the-sdram/3389/4
class StereoBufferChunk {
private:
    float* bufferL;
    float* bufferR;

    uint32_t start;
    uint32_t end;
    uint32_t size;

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

    uint32_t toMainBufferIndex(uint32_t index)const{
        return {index + start};
    }

public:
    StereoBufferChunk(float* bufL, float* bufR, uint32_t startIndex, uint32_t endIndex) 
        : bufferL(bufL), bufferR(bufR), start(startIndex), end (endIndex), size(endIndex - startIndex) {}
    
    void setValue(uint32_t index, float left, float right) {
            bufferL[index] = left;
            bufferR[index] = right;
    }


    StereoPair getSample(uint32_t index) const{
        return {getValueInterpolated(toMainBufferIndex(index))};
    }
        

};
