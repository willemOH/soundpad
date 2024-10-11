#pragma once

struct StereoPair{
        float left;
        float right;
    };

struct StereoPairRef{
    float& left;
    float& right;
    StereoPairRef(float& l, float& r) : left(l), right(r) {}
};