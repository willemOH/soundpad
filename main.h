#pragma once

#include "daisysp.h"

struct SampleSettings
{
	// sample
	// sssssssssssssssssssss
	// start lstart lend end
	// gate--<--------->
	uint32_t sLength; // length of sample, < BUFFER_MAX
	uint32_t sPhaseStart;
	uint32_t sPhaseLoopStart;
	uint32_t sPhaseLoopEnd;
	uint32_t sPhaseEnd;
};