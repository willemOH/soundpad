#pragma once

#include "daisysp.h"
//using namespace daisy;
using namespace daisysp; 

// cannot define buffers to SDRAM within an object: https://forum.electro-smith.com/t/possible-runtime-crash-trying-to-put-an-object-in-the-sdram/3389/4
class StereoBufferChunk {
private:
    float* buffer;

    uint32_t start;
    uint32_t end;
    uint32_t size;

    float getValueInterpolated(float index) const{ 
        int32_t indexInt = static_cast<int32_t>(index); //strips decimal
        float indexFraction = index - indexInt; //gets decimal

        float currentChannel = getValue(indexInt);
        float nextChannel = getValue(indexInt + 2); //if currentChannel is left channel, nextChannel will be left channel. Same with right
        
        // Linear interpolation (value halfway between samples)
        float sig = currentChannel + (nextChannel - currentChannel) * indexFraction;

        return {sig}; 
    }

    float getValue(uint32_t index) const { //const = readonly
            return {buffer[index]};
    } 

    uint32_t toMainBufferIndex(uint32_t index)const{
        return {index + start};
    }

public:
    StereoBufferChunk(float* buf, uint32_t startIndex, uint32_t endIndex) 
        : buffer(buf), start(startIndex), end (endIndex), size(endIndex - startIndex) {}
    
    void setValue(uint32_t index, float left, float right) { //currently does not limit setValue to start and end indexes
            buffer[index] = left;
            buffer[index + 1] = right;
    }


    float getSample(uint32_t index) const{ 
        return {getValueInterpolated(toMainBufferIndex(index))};
    }
        

};
