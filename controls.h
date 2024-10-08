#ifndef CONTROLS_H
#define CONTROLS_H
#include "daisy_seed.h"
#include "sp_ui.h" // Include the UI header directly
#include <vector>

using namespace daisy;

class Controls {
public:
    void Init(SPUI* uiInstance); // Pass a pointer to the UI class
    void UpdateControlStates();

private:
    void DebounceButtons();
    std::vector<Buttons> pressedButtons;
    //Buttons pressedButton[2];
    DaisySeed hardware;
    Switch button[2];
    AdcChannelConfig adcConfig[2]; //pots
    //pot update parameters
    float pot1old = 0.0f;
    float pot2old = 0.0f;
    float threshold = 0.008f; //so that small flucuations of adc read don't trigger a change in start and end
                                //maybe rename to minimumTrigThreshold
                                //could be lower if pot connections pick up less noise on pcb
                                //ideally another threshold is set in ui or sample so that it can be tweaked depending on specific control
    std::array<bool, 4> potTrigs = {false, false, false, false};
    SPUI* ui; // Store a pointer to the UI class
};

#endif // CONTROLS_H