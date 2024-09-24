#include "daisy_seed.h"
#include "main.h"
#include "core_cm7.h" //measuring
#include "stereo_buffer_chunk.h"
#include "sample.h"
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
StereoBufferChunk Buffer(BufferL, BufferR, 48000, 2 * 48000);

int soundSeconds = 2;
StereoBufferChunk* soundBuffers[16];

Sample sample;
//BufferSubStereo pageBuffer;


bool record;

enum millisecondDivisions : uint32_t{ // can be called without using millisecondDivisions.value. just value. Move scope into class or use 'enum class'
	one = 1000,
	two = 2000,
	three = 3000,
	max = 3500
};

void AllocateBufferChunks(millisecondDivisions division){
	uint32_t start = 0;
	for(int i=0; i<16; i++){
		soundBuffers[i] = new StereoBufferChunk(BufferL, BufferR, start * 48, (start + start) * 48);
		start += division;
	}
} 


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{	
	//float sigInR, sigInL, sigOutR, sigOutL = 0.0f;
	float sigR, sigL = 0.0f;
	//Playback(in*, out*, size); //soon to be a separate class from record
	
    //float sigGate;
    
	// measure MCU utilization
	#ifdef MEASURE
	// measure - start
	DWT->CYCCNT = 0;
	#endif

	// audio

    for (size_t i = 0; i < size; i += 2)
    {
		sigL = in[i];
		sigR = in[i + 1];	
		sample.Process(sigL, sigR, record);
		out[i]  = sample.GetOutput().left;
        out[i + 1] = sample.GetOutput().right;
		
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
	
	
	AllocateBufferChunks(two);
	sample.Init(sysSampleRate, soundBuffers[0]);

	System::Delay(100);
    // sampler - setup

	// logging over serial USB
	#ifdef LOGG
	hardware.StartLog(true); // start log but don't wait for PC - we can be connected to a battery
	hardware.PrintLine("monitoring started");
	#endif
	
	//sGate = false;
	//sGatePrev = false;
	

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
		sample.RecordPrepare(recButton.RisingEdge());
        record = recButton.Pressed();
        System::Delay(1);
    }
}
