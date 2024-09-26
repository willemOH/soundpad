#include "sp_ui.h"
//#include "globals.h" // Include globals.h to access hardware

void SPUI::Init(ISample* sampleInstance) {
    sample = sampleInstance;
}

void SPUI::Update(std::vector<Buttons>& pressedButtons) {
    if (!pressedButtons.empty()) {
        switch (pressedButtons[0]) {
            case Buttons::BUTTON1: //record
                if(once){
                    sample->RecordPrepare();
                }
                play = false;
                sample->SetRecord(true);
                once = false;
                break;//early exit otherwise use flags to activate functionality
            case Buttons::BUTTON2:
                play=!play;
                goto second_button;
            default:
                // Handle unexpected values for the first element
                break;
        }
    }
    else{
        sample->SetRecord(false);
        once = true;
    }
    second_button:
        if (pressedButtons.size() > 1) {
            switch (pressedButtons[1]) {
                case Buttons::BUTTON2: 
                    loop=!loop;
                    break;
                default:
                    // Handle unexpected values for the second element
                    break;
            }
        }
    //hardware.PrintLine("recordStart = %s", recordStart ? "true" : "false");
}