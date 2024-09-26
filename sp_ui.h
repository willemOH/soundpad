#ifndef SPUI_H
#define SPUI_H

#include <array>
#include <vector>
//#include "main.h"
#define UI_MODE_SOUND 1
#define MAX_OBSERVERS 2


enum class Buttons {
        BUTTON1,
        BUTTON2,
    };

class SPUI { //libDaisy already has a UI class - unnecessary UI display functions
public:
    /* void AddObservers(std::array<Observer*, MAX_OBSERVERS> observers) {
        observers_ = observers;
    }  *///could be constructor but observers are optional right now
    void Update(std::vector<Buttons>& pressedButtons);

   /*  struct UiEvents{ //all ui events that controls can activate
        uint16_t record = UiEventQueue::invalidButtonId;
    }; */
    bool once = true;
    bool recordStart = false;
    bool record = false;
    bool play = false;
    bool loop = false;

private:
    //const std::vector<Buttons>& pressedButtons_; //Store the reference
};

#endif // SPUI_H