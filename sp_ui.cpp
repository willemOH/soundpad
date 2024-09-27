#include "sp_ui.h"
#include "global.h" // Include globals.h to access hardware

void SPUI::Init(ISample* sampleInstance) {
    sample = sampleInstance;
}

void SPUI::Update(std::vector<Buttons>& pressedButtons) {
    if (!pressedButtons.empty()) {
        switch (pressedButtons[0]) {
            case Buttons::BUTTON1: //record
                if(once){
                    sample->RecordPrepare();
                    once = false;
                }
                sample->SetRecord(true);;
                
                break;//early exit otherwise use flags to activate functionality
            case Buttons::BUTTON2: //play
                if(once){
                    sample->PlayPrepare();
                    once = false;
                }
                sample->SetPlayback(true);
                goto second_button;
            default:
                // Handle unexpected values for the first element
                break;
        }
    }
    else{
        sample->SetRecord(false);
        sample->SetPlayback(false);
        once = true;
    }
    second_button:
        if (pressedButtons.size() > 1) {
            switch (pressedButtons[1]) {
                case Buttons::BUTTON1: 
                    hardware.PrintLine("setloop reached");
                    sample->SetLoop(true); 
                    break;
                default:
                    // Handle unexpected values for the second element
                    break;
            }
        }
}