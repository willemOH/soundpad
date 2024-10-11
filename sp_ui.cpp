#include "sp_ui.h"
#include "global.h" // Include globals.h to access hardware

void SPUI::Init(ISample* sampleInstance) {
    sample = sampleInstance;
    input = true; //to be sure `else if` of update() sets bool states to false which are being set after samples init somehow; hacky 
}

void SPUI::Update(std::vector<Buttons>& pressedButtons, float& slider1, float& slider2, float& slider3, float& slide4, std::array<bool, 4> sliderTrigs) {
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
                if(sliderTrigs[0]){
                    sample->SetSpeed(slider1);
                }
                if(sliderTrigs[1]){
                    sample->SetNote(slider2);
                }
                goto combo;
            default:
                // Handle unexpected values for the first element
                break;
        }
    }
    else if(input){
        sample->SetRecord(false);
        sample->SetPlayback(false); //-<-
        input = false;
    }
    else{
        once = true;
         //sliders
    if(sliderTrigs[0] || sample->GetLoopState()){
    sample->SetStart(slider1); //controls will send a lot of triggers for whenever pot moves. when triggered is false for more time than it takes to reset when sliding reasonably fast, preview = true
    } 
    if(sliderTrigs[1] || sample->GetLoopState()){
    sample->SetEnd(slider2);
    }
    }
    combo:
        if (pressedButtons.size() > 1) { //second button
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