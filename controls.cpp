#include "controls.h"

void Controls::Init(SPUI* uiInstance){
    ui = uiInstance;
    adcConfig[0].InitSingle(hardware.GetPin(15)); 
    adcConfig[1].InitSingle(hardware.GetPin(16)); 
    hardware.adc.Init(adcConfig, 2);
    hardware.adc.Start();
    if (i2c_transport.Init(config.transport_config)){
        hardware.PrintLine("I2C Transport Initialization Failed");
    }
    if (mpr121.Init(config) != Mpr121<Mpr121I2CTransport>::Result::OK){
        hardware.PrintLine("MPR121 Initialization Failed");
    }
}

void Controls::UpdateControlStates() {
//sliders
    float pot1 = hardware.adc.GetFloat(0);
    float pot2 = hardware.adc.GetFloat(1);
    float pot3 = 0;
    float pot4 = 0;

    if(abs(pot1 - pot1old) > threshold){
    //hardware.PrintLine("pot1 %d", static_cast<int>(pot1 * 10000));
    pot1old = pot1;
    potTrigs[0]=true;
    }
   
    if(abs(pot2 - pot2old) > threshold){
    //hardware.PrintLine("pot2 %d", static_cast<int>(pot2 * 10000));
    pot2old = pot2;
    potTrigs[1]=true;
    }
//buttons
    uint16_t buttonsState = mpr121.Touched();
        if(buttonsState != old_status){
            //hardware.PrintLine("touch status: %u", buttonsState);
            processNewButtonsState(buttonsState);
            //hardware.PrintLine("pressedButtonsState: [%d,%d]", pressedButtons[0], pressedButtons[1]);
            old_status = buttonsState;
        }
    #ifdef LOGG
    if(edge){
    // Print the contents of pressedButtons
    hardware.PrintLine("Contents of pressedButtons:");
    for (size_t i = 0; i < pressedButtons.size(); i++) {
        hardware.PrintLine("pressedButtons[%u] = %u", static_cast<unsigned int>(i), static_cast<unsigned int>(pressedButtons[i]));
    }
    }
    #endif
    // Directly calls UI method with the pressed buttons list
    ui->Update(pressedButtons, pot1, pot2, pot3, pot4, potTrigs);
    potTrigs = {false, false, false, false};
}

void Controls::processNewButtonsState(uint16_t buttonsState)
{
   if(pressedButtons[0]){ //there are any pressed buttons (index 1 must be full before index 2 can be)
        uint16_t change = oldButtonsState ^ buttonsState;
        if(change & oldButtonsState){ //change is subtraction
            RemoveButton(LSSBtoButton(change));
        }
        else{ //change is addition
            if(pressedButtons[1]){ 
                return; //no pressed buttons added if already two pressed
            }
            AddButton(LSSBtoButton(change));
        }
    }
    else{
        AddButton(LSSBtoButton(buttonsState));
    }
    oldButtonsState = buttonsState;
}

void Controls::RemoveButton(uint8_t button){
    if(pressedButtons[0] == button){
         pressedButtons[0] = pressedButtons[1]; //remove first button by popping second value to first
         pressedButtons[1] = 0; //second button is removed (updated to current state)
    }
    else if(pressedButtons[1] == button){
        pressedButtons[1] = 0;
    }
    else{
        hardware.PrintLine("remove Button error: button %d not found in pressedButtons", button);
        hardware.PrintLine("pressedButtons[0]: %d, pressedButtons[1]: %d", pressedButtons[0], pressedButtons[1]);
        }
}

void Controls::AddButton(uint8_t button){
    if(!pressedButtons[0]){
        pressedButtons[0] = button;
    }
    else if(!pressedButtons[1]){
        pressedButtons[1] = button; 
    }
    else{
        hardware.PrintLine("add Button error: both positions in pressedButtons are occupied");
        hardware.PrintLine("pressedButtons[0]: %d, pressedButtons[1]: %d", pressedButtons[0], pressedButtons[1]);
        }
}

uint8_t Controls::LSSBtoButton(uint16_t number) {
    for (int i = 0; i < 16; ++i) { // Assuming a 16-bit number
        if (number & (1 << i)) {
            return i + 1;
        }
    }
    hardware.PrintLine("no bits set, LSSB error");
    return 0; // Return -1 if no bits are set
}