#include "daisy_seed.h"
#include "main.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware
DaisySeed hardware;

float sysSampleRate;
float sysCallbackRate;

// sampler
#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 
float DSY_SDRAM_BSS sBufferR[BUFFER_MAX];
float DSY_SDRAM_BSS sBufferL[BUFFER_MAX];

float sIndex; // index into buffer
uint32_t sIndexInt;
float sIndexFraction;
uint32_t sIndexRecord;
float sFactor; // how much to advance index for a new sample
float sFreq; // in Hz
bool sGate, sGatePrev; //
SampleSettings sampleSettings;
bool record;


// demo
void fillBuffer()
{
	Oscillator osc;

    osc.Init(sysSampleRate);
    osc.SetWaveform(osc.WAVE_TRI);
	osc.SetFreq(440.0f);
	osc.SetAmp(0.1f);

	for (uint32_t i = 0; i < BUFFER_MAX; i++)
	{
		sBufferR[i] = osc.Process();
		sBufferL[i] = sBufferR[i];	
	}
}

void RecordPrepare(bool yes)
{
	if(yes){
		sIndexRecord = 0;
		sampleSettings.sPhaseStart = 0;
		sampleSettings.sPhaseLoopStart = 0;
	}
}

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
	float sigR, sigL = 0.0f;
    float a, b;
    //float sigGate;
    

	// audio

    for (size_t i = 0; i < size; i += 2)
    {

		// record

		if (record)
		{
			// input
				sigL = in[i];
				sigR = in[i + 1];			

				if (sIndexRecord < (BUFFER_MAX - 1))
				{
					sBufferL[sIndexRecord] = sigR;
					sBufferR[sIndexRecord] = sigL;
					sIndexRecord++;

					sampleSettings.sPhaseLoopEnd = sIndexRecord;
					sampleSettings.sPhaseEnd = sIndexRecord;
					sampleSettings.sLength = sIndexRecord;
				}
		
			
			// pass through
			out[i] = sigR;
			out[i + 1] = sigL;

		} else {

				sIndex = sampleSettings.sPhaseStart;		

				if (sIndex < sampleSettings.sPhaseEnd)
				{
					sIndexInt = static_cast<int32_t>(sIndex); //strips decimal
					sIndexFraction = sIndex - sIndexInt;

					// get sample
					a = sBufferL[sIndexInt];
					b = sBufferL[sIndexInt + 1];
					sigL = a + (b - a) * sIndexFraction; //linear interpolation (get value halfway between samples)
					a = sBufferR[sIndexInt];
					b = sBufferR[sIndexInt + 1];
					sigR = a + (b - a) * sIndexFraction;

					sIndex += sFactor;
					if (sIndex >= sampleSettings.sPhaseLoopEnd)
					{
						sIndex = sampleSettings.sPhaseLoopStart;
					}
					out[i]  = sigL;
		    		out[i + 1] = sigR;
				}
				out[i]  = sigL;
		    	out[i + 1] = sigR;
		}
    }

}

int main(void)
{
    hardware.Configure();
    hardware.Init();
    
    sysSampleRate = hardware.AudioSampleRate();
	sysCallbackRate = hardware.AudioCallbackRate();
    
    Switch recButton;
    Switch playButton;
    recButton.Init(hardware.GetPin(29), 1000);
    playButton.Init(hardware.GetPin(30), 1000);
    
	System::Delay(100);
    // sampler - setup
	fillBuffer();
	sIndex = 0.0f;
	sFreq = 440.0f;
	sFactor = (sFreq / 440.0f);
	//sGate = false;
	//sGatePrev = false;
	sampleSettings.sPhaseStart = 48000.0f * 0.0f;
	sampleSettings.sPhaseLoopStart = 48000.0f * 0.1f;
	sampleSettings.sPhaseLoopEnd = 48000.0f * 0.5f;
	sampleSettings.sPhaseEnd = 48000.0f * 1.0f;
	sampleSettings.sLength = sampleSettings.sPhaseEnd;

	record = false;
	// start callback
    hardware.StartAudio(AudioCallback);

    //update loop
    for(;;)
    {
        recButton.Debounce();
        hardware.SetLed(recButton.Pressed());
		RecordPrepare(recButton.Pressed());
        record = recButton.Pressed();
        System::Delay(1);
    }
}
