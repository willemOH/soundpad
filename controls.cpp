#include "controls.h"

void Controls::Init(SPUI* uiInstance){
    ui = uiInstance;
    button[0].Init(hardware.GetPin(29), 1000);
    button[1].Init(hardware.GetPin(30), 1000);
}

//will be separate 
void Controls::UpdateControlStates() {
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
    ui->Update(pressedButtons);
    hardware.PrintLine("controls.cpp - once = %d, UI instance address: %p", ui->once, static_cast<void*>(ui));
}

void Controls::DebounceButtons(){
    for(uint8_t i=0; i < sizeof(button) / sizeof(button[0]); i++){
        button[i].Debounce();
    }
}


/* 
 controls.recButton.Debounce();
        hardware.SetLed(controls.recButton.Pressed());
		RecordPrepare(controls.recButton.RisingEdge());
        record = controls.recButton.Pressed(); */