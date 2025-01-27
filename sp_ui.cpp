#include "sp_ui.h"
#include "global.h" // Include globals.h to access hardware

constexpr int toInt(Buttons button) {
    return static_cast<int>(button);
}

void SPUI::Init(ISample* sampleInstance) {
    sample = sampleInstance;
    input = true; //to be sure `else if` of update() sets bool states to false which are being set after samples init somehow; hacky 
}

void SPUI::Update(uint8_t pressedButtons[2], float& slider1, float& slider2, float& slider3, float& slider4, std::array<bool, 4> sliderTrigs) {
     //hardware.PrintLine("pressedButtonsState: [%d,%d]", pressedButtons[0], pressedButtons[1]); 
    /* if(sliderTrigs[1]){
        sample->SetNote(slider2);
    } */
    if (pressedButtons[0]){ //array is not empty (values shift towards first index)
        input = true;
        switch (pressedButtons[0]) {
             case toInt(Buttons::Pocket): //record
                if(once){
                    sample->RecordPrepare();
                    once = false;
                    sample->SetRecord(true);
                }
             mLog(BUTTON, "pressed: L5, record");
                break;
            case toInt(Buttons::Play): //play
                if(once){
                    sample->PlayPrepare();
                    once = false;
                    sample->SetPlayback(true);
                }
                mLog(BUTTON, "pressed: L6 (temp loc) play");
                if(sliderTrigs[0]){ //TODO: this set speed should not restart sample
                    sample->SetSpeed(slider1);}
                break;
            case toInt(Buttons::R1): //set start
                if(sliderTrigs[0]){
                    sample->SetStart(slider1); //controls will send a lot of triggers for whenever pot moves. when triggered is false for more time than it takes to reset when sliding reasonably fast, preview = true
                } 
                 mLog(BUTTON, "pressed: R1, set start");
                break;
            case toInt(Buttons::R2): //set end
                if(sliderTrigs[1]){
                    sample->SetEnd(slider2);
                }
                 mLog(BUTTON, "pressed: R2, set end");
                break;
            case toInt(Buttons::R3): //loop off
                if (pressedButtons[1]) { //second pressed
                    switch (pressedButtons[1]) {
                        case toInt(Buttons::L1): //group 1
                        sample->SetLoop(false);
                        mLog(BUTTON, "pressed: loop off");
                        break;
                    default:
                        break;
                    }
                }
                break;  
            case toInt(Buttons::R4): //loop on
                if (pressedButtons[1]) { //second pressed
                    switch (pressedButtons[1]) {
                        case toInt(Buttons::L1): //group 1
                        sample->SetLoop(true); 
                        mLog(BUTTON, "pressed: loop on");
                        break;
                    default:
                        break;
                    }
                }
                break; 
            default:
                break;
        }
    }
    else if(input){
        sample->SetRecord(false);
        sample->SetPlayback(false); //-<-
        input = false;
    }
    else{
         //sliders
         if(sliderTrigs[0]){ //TODO: remove zippering: check for trig but then setspeed (smoothly) until gate is set off (some amount of time as a virtual keypress on slider trigger)
            sample->SetSpeed(slider1); //if settings.loop then playback wont stop till press play again. TODO: implement gate as substitute for settings playback, adsr etc.
            }
        once = true;
    }

}