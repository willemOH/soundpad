#ifndef SPUI_H
#define SPUI_H

#include <array>
#include <vector>
#include "i_sample.h"
#define UI_MODE_SOUND 1
#define MAX_OBSERVERS 2

class SPUI { //libDaisy already has a UI class - unnecessary UI display functions
public:

    void Init(ISample* sampleInstance);
    void Update(uint8_t pressedButtons[2], float& slider1, float& slider2, float& slider3, float& slide4, std::array<bool, 4> sliderTrigs);
    bool once = true;
    bool record = false;
    bool play = false;
    bool loop = false;

private:
    bool input = false;
    ISample* sample;
};

#endif // SPUI_H