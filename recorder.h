#ifndef RECORDER_H
#define RECORDER_H

#include "daisy_seed.h"
#include "stereo_buffer_chunk.h"
#include <cstdint>
#include <functional>

#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 
// but the buffer max will be the szie of a chunk, which may be static but not sure now

using namespace daisy;

class Recorder {

public:

    Recorder(float& left, float& right) : inputRef(left, right) {} //references must be intialized upon construction

    uint32_t indexRecord = 0;

    void Record() { //std::function for callback function returning uint type
        if (indexRecord < (BUFFER_MAX - 1)) {
            sBuffer->setValue(indexRecord, inputRef.left, inputRef.right);
            indexRecord++;
        }
    }

private:
    struct StereoPairRef{
        float& left;
        float& right;
        StereoPairRef(float& l, float& r) : left(l), right(r) {}
    };

    StereoPairRef inputRef;
    StereoBufferChunk* sBuffer;
};

#endif // RECORDER_H
