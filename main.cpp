#include "daisy_seed.h"
#include "main.h"
#include "core_cm7.h" //measuring
#include "bufferStereo.h"
// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

//#define LOGG // start serial over USB Logger class
//#define MEASURE // measure MCU utilization

DaisySeed hardware;

float sysSampleRate;
float sysCallbackRate;

// Global variables for debugging
volatile bool printFlag = false;
volatile float sIndexDebug = 0.0f;
volatile float loopstartdebug = 0.0f;
volatile uint32_t sIndexRecordDebug = 0.0f;

// sampler
#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 
float DSY_SDRAM_BSS BufferR[BUFFER_MAX];
float DSY_SDRAM_BSS BufferL[BUFFER_MAX];
BufferStereo Buffer(BufferL, BufferR, 48000, 2 * 48000);

int soundSeconds = 2;
BufferStereo* soundBuffers[16];
//BufferSubStereo pageBuffer;

float sIndex; // index into buffer
uint32_t sIndexInt;
float sIndexFraction;
uint32_t sIndexRecord;
float sFactor; // how much to advance index for a new sample
float sFreq; // in Hz
bool sGate, sGatePrev; //
SampleSettings sampleSettings;
bool record;

enum millisecondDivisions : uint32_t{ // can be called without using millisecondDivisions.value. just value. Move scope into class or use 'enum class'
	one = 1000,
	two = 2000,
	three = 3000,
	max = 3500
};

void AllocateSubBuffers(millisecondDivisions division){
	uint32_t start = 0;
	for(int i=0; i<16; i++){
		soundBuffers[i] = new BufferStereo(BufferL, BufferR, start * 48, (start + start) * 48);
		start += division;
	}
} 

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
		float signal = osc.Process();
		Buffer.setValue(i, signal, signal);
	}
	// Print the size of sBufferL
    size_t sizeOfsBufferL = sizeof(BufferL) / sizeof(BufferL[0]);
 hardware.PrintLine("Size of sBufferL: %u", static_cast<unsigned int>(sizeOfsBufferL));

}

void RecordPrepare(bool yes)
{
	if(yes){
		sIndexRecord = 0;
		sampleSettings.sPhaseStart = 0;
		sampleSettings.sPhaseLoopStart = 0;
	}
}

// Function to print sIndex value outside the AudioCallback
void PrintDebugInfo()
{
    if (printFlag)
    {
        int loopstartdebugInt = static_cast<int>(loopstartdebug);
       hardware.PrintLine("loopstart = %d", sampleSettings.sPhaseLoopStart);
    	printFlag = false;
    }
}

void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
	float sigR, sigL = 0.0f;
    //float sigGate;
    
	// measure MCU utilization
	#ifdef MEASURE
	// measure - start
	DWT->CYCCNT = 0;
	#endif

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
					(*soundBuffers[0]).setValue(sIndexRecord, sigL, sigR);
					sIndexRecord++;

					sampleSettings.sPhaseLoopEnd = sIndexRecord;
					sampleSettings.sPhaseEnd = sIndexRecord;
					sampleSettings.sLength = sIndexRecord;

					sIndexRecordDebug = sIndexRecord;
					printFlag = true;
				}
			
			// pass through
			out[i] = sigR;
			out[i + 1] = sigL;
		} 
		else {	
				if (sIndex < sampleSettings.sPhaseEnd)
				{
					sigL = (*soundBuffers[0]).getSample(sIndex).left;
					sigR = (*soundBuffers[0]).getSample(sIndex).right;

					sIndex += 1.0;
					//sIndex += sFactor;
					if (sIndex >= sampleSettings.sPhaseLoopEnd)
					{
						sIndex = sampleSettings.sPhaseLoopStart;
					}
					out[i]  = sigL;
		    		out[i + 1] = sigR;
				#ifdef LOGG
                printFlag = true;
				#endif
				}
				else{
					printFlag = true;
					sIndex = 0;
					sIndexDebug = sIndex;
					//loopstartdebug = sampleSettings.sPhaseEnd;
				}
		}
	
	// measure MCU utilization
   	#ifdef MEASURE
	// measure - stop
	if (DWT->CYCCNT > 390000)
	{
		hardware.SetLed(true);
	}
	#endif
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
	
	AllocateSubBuffers(two);
	System::Delay(100);
    // sampler - setup

	// logging over serial USB
	#ifdef LOGG
	hardware.StartLog(true); // start log but don't wait for PC - we can be connected to a battery
	hardware.PrintLine("monitoring started");
	#endif
	
	fillBuffer();
	sIndex = 0.0f;
	sFreq = 440.0f;
	sFactor = (sFreq / 440.0f);
	//sGate = false;
	//sGatePrev = false;
	sampleSettings.sPhaseStart = 48000.0f * 0.0f;
	sampleSettings.sPhaseLoopStart = 48000.0f * 0.0f;
	sampleSettings.sPhaseLoopEnd = 48000.0f * 0.5f;
	sampleSettings.sPhaseEnd = 48000.0f * 1.0f;
	sampleSettings.sLength = sampleSettings.sPhaseEnd;

	record = false;
	// start callback
    hardware.StartAudio(AudioCallback);

    //update loop
    for(;;)
    { 
		//hardware.PrintLine("float test: %f", 100.00f); 
		#ifdef LOGG
		PrintDebugInfo();
		#endif
        recButton.Debounce();
        hardware.SetLed(recButton.Pressed());
		RecordPrepare(recButton.RisingEdge());
        record = recButton.Pressed();
        System::Delay(1);
    }
}
