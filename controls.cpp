#include "controls.h"

void Controls::Init(SPUI* uiInstance){
    ui = uiInstance;
    button[0].Init(hardware.GetPin(29), 1000);
    button[1].Init(hardware.GetPin(30), 1000);
    adcConfig[0].InitSingle(hardware.GetPin(15)); 
    adcConfig[1].InitSingle(hardware.GetPin(16)); 
    hardware.adc.Init(adcConfig, 2);
    hardware.adc.Start();
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
    }

    if(abs(pot2 - pot2old) > threshold){
    //hardware.PrintLine("pot2 %d", static_cast<int>(pot2 * 10000));
    pot2old = pot2;
    }
//buttons
    bool edge = false;
    DebounceButtons();
    for (uint8_t i = 0; i < sizeof(button) / sizeof(button[0]); i++) {
        if (button[i].RisingEdge()) {
            #ifdef LOGG
            edge = true;
            hardware.PrintLine("button %i just pressed", i);
            #endif
            pressedButtons.push_back(static_cast<Buttons>(i));
            //hardware.PrintLine("in list at %i: %u", i, static_cast<unsigned int>(pressedButtons.at(i))); 
        }
        else if(button[i].FallingEdge()){
            #ifdef LOGG
            edge = true;
            hardware.PrintLine("button %i just released", i);
            #endif
            //finds enum of button in list that has falling edge and removes it from list if found
            auto it = std::find(pressedButtons.begin(), pressedButtons.end(), static_cast<Buttons>(i)); 
            if (it != pressedButtons.end()) {
                pressedButtons.erase(it);
            }
        }
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
    ui->Update(pressedButtons, pot1, pot2, pot3, pot4);
}

void Controls::DebounceButtons(){
    for(uint8_t i=0; i < sizeof(button) / sizeof(button[0]); i++){
        button[i].Debounce();
    }
}

