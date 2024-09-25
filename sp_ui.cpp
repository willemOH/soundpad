#include "sp_ui.h"
//#include "globals.h" // Include globals.h to access hardware

/* SPUI::SPUI(std::array<Observer*, MAX_OBSERVERS> observers)
    : observers_(observers) {}
 */
void SPUI::NotifyObservers() {
    for (Observer* observer : observers_) {
        if (observer) {
            observer->Update();
        }
    }
}
void SPUI::Update(std::vector<Buttons>& pressedButtons) {
    if (!pressedButtons.empty()) {
        switch (pressedButtons[0]) {
            case Buttons::BUTTON1: //record
                if(once){
                    recordStart = true;
                    //hardware.PrintLine("recordStart = true");
                }
                else{
                    recordStart = false;
                    //hardware.PrintLine("recordStart = false");
                }
                play = false;
                record = true;
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
        record=false;
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