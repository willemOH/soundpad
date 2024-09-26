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
    SPUI* ui; // Store a pointer to the UI class
};

#endif // CONTROLS_H