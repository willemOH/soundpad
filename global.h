#ifndef GLOBAL_H
#define GLOBAL_H
#include "daisy_seed.h"
#include <cstdarg> // For va_list, va_start, va_end
#include <cstdio>  // For vsnprintf
using namespace daisy;

extern DaisySeed hardware;

// categories as bit flags
enum LogCategory {
    GENERAL    = 1 << 0,
    BUTTON     = 1 << 1,
    AUDIO      = 1 << 2,
	SD_CARD    = 1 << 3,
    ALL        = 0xFFFFFFFF //not to be selected. Rather, if used in mLog(), will always show
};
//LOGG filter tool

// bitmask, comment out to disable monitoring, | between categories
#define LOGG (GENERAL | BUTTON | SD_CARD) // start serial over USB Logger class

//inline to not generate multiple definitions
inline void mLog(LogCategory category, const char* format, ...) {
    #ifdef LOGG
    if (category & LOGG) {
        char buffer[256];
        va_list args;
        va_start(args, format);
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);
        hardware.PrintLine("%s", buffer);
    }
    #endif
}

#endif // GLOBAL_H