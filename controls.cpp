#include "controls.h"

void Controls::Init(SPUI* uiInstance){
    ui = uiInstance;
    adcConfig[0].InitSingle(hardware.GetPin(15)); 
    adcConfig[1].InitSingle(hardware.GetPin(16));
    adcConfig[2].InitSingle(hardware.GetPin(17)); 
    adcConfig[3].InitSingle(hardware.GetPin(18));  
    hardware.adc.Init(adcConfig, 4);
    hardware.adc.Start();
    if (i2c_transport.Init(config.transport_config)){
        mLog(ALL,"I2C Transport Initialization Failed");
    }
    if (mpr121.Init(config) != Mpr121<Mpr121I2CTransport>::Result::OK){
        mLog(BUTTON, "MPR121 Initialization Failed");
    }
}

void Controls::UpdateControlStates() {
//sliders
    float pot1 = hardware.adc.GetFloat(0);
    float pot2 = hardware.adc.GetFloat(1);
    float pot3 = hardware.adc.GetFloat(2);
    float pot4 = hardware.adc.GetFloat(3);

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

    if(abs(pot3 - pot3old) > threshold){
    //hardware.PrintLine("pot1 %d", static_cast<int>(pot1 * 10000));
    pot3old = pot3;
    potTrigs[2]=true;
    }
   
    if(abs(pot4 - pot4old) > threshold){
    //hardware.PrintLine("pot2 %d", static_cast<int>(pot2 * 10000));
    pot4old = pot4;
    potTrigs[3]=true;
    }
//buttons
    uint16_t buttonsState = mpr121.Touched();
        if(buttonsState != old_status){
            //hardware.PrintLine("touch status: %u", buttonsState);
            processNewButtonsState(buttonsState);
            //hardware.PrintLine("pressedButtonsState: [%d,%d]", pressedButtons[0], pressedButtons[1]);
            old_status = buttonsState;
        }
    // Directly calls UI method with the pressed buttons list
    ui->Update(pressedButtons, pot1, pot2, pot3, pot4, potTrigs);
    potTrigs = {false, false, false, false};
}

void Controls::processNewButtonsState(uint16_t buttonsState)
{
   if(pressedButtons[0]){ //there are any pressed buttons (index 1 must be full before index 2 can be)
        uint16_t change = oldButtonsState ^ buttonsState;
        if(!buttonsState){ //clear stuck buttons, if function tries to remove a stuck button, it gets bad, so check first
            pressedButtons[0] = 0;
            pressedButtons[1] = 0;
            mLog(BUTTON, "stuck button cleared");
        }
        else if(change & oldButtonsState){ //change is subtraction
            RemoveButton(LSSBtoButton(change));
        }
        else{ //change is addition
            //cannot return here if 3rd, nth button. Instead deflect in addbutton or will be blind and unready
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
        mLog(BUTTON, "remove Button error: button %d not found in pressedButtons", button);
        mLog(BUTTON, "pressedButtons[0]: %d, pressedButtons[1]: %d", pressedButtons[0], pressedButtons[1]);
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
        mLog(BUTTON, "add Button error: both positions in pressedButtons are occupied");
        mLog(BUTTON, "pressedButtons[0]: %d, pressedButtons[1]: %d", pressedButtons[0], pressedButtons[1]);
        }
}

uint8_t Controls::LSSBtoButton(uint16_t number) {
    for (int i = 0; i < 16; ++i) { // Assuming a 16-bit number
        if (number & (1 << i)) {
            return i + 1;
        }
    }
    mLog(BUTTON, "no bits set, LSSB error");
    return 0; // Return -1 if no bits are set
}