#include "daisy_seed.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;

// Declare a DaisySeed object called hardware
DaisySeed hardware;

int main(void)
{
    hardware.Configure();
    hardware.Init();
    
    Switch recButton;
    Switch playButton;
    recButton.Init(hardware.GetPin(29), 1000);
    playButton.Init(hardware.GetPin(30), 1000);
    
    //update loop
    for(;;)
    {
        recButton.Debounce();
        hardware.SetLed(recButton.Pressed());

        System::Delay(1);
    }
}
