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
    DaisySeed hardware;
    SPUI* ui; 
    //capacitive buttons
    Mpr121I2CTransport i2c_transport;
    Mpr121<Mpr121I2CTransport>::Config config;
    Mpr121<Mpr121I2CTransport> mpr121;
    uint16_t old_status;
    uint8_t pressedButtons[2];
    uint16_t oldButtonsState;
    void RemoveButton(uint8_t index);
    void AddButton(uint8_t index);
    uint8_t LSSBtoButton(uint16_t number);
    void processNewButtonsState(uint16_t ButtonsState);
    //potentiometers
    AdcChannelConfig adcConfig[2];
    float pot1old = 0.0f;
    float pot2old = 0.0f;
    float threshold = 0.008f; //so that small flucuations of adc read don't trigger a change in start and end
                                //maybe rename to minimumTrigThreshold
                                //could be lower if pot connections pick up less noise on pcb
                                //ideally another threshold is set in ui or sample so that it can be tweaked depending on specific control
    std::array<bool, 4> potTrigs = {false, false, false, false};
};

#endif // CONTROLS_H