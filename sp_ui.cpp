#include "sp_ui.h"
#include "global.h" // Include globals.h to access hardware

void SPUI::Init(ISample* sampleInstance) {
    sample = sampleInstance;
}

void SPUI::Update(std::vector<Buttons>& pressedButtons, float& slider1, float& slider2, float& slider3, float& slide4) {
    if (!pressedButtons.empty()) {
        input = true;
        switch (pressedButtons[0]) {
            case Buttons::BUTTON1: //record
                if(once){
                    sample->RecordPrepare();
                    once = false;
                    sample->SetRecord(true);
                }
                break;//early exit otherwise use flags to activate functionality
            case Buttons::BUTTON2: //play
                if(once){
                    sample->PlayPrepare();
                    once = false;
                    sample->SetPlayback(true);
                }
                goto second_button;
            default:
                // Handle unexpected values for the first element
                break;
        }
    }
    else if(input){
        sample->SetRecord(false);
        sample->SetPlayback(false);
        input = false;
    }
    else{
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

    //sliders
    sample->SetStart(slider1); //controls will send a lot of triggers for whenever pot moves. when triggered is false for more time than it takes to reset when sliding reasonably fast, preview = true
    sample->SetEnd(slider2);
}