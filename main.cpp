#include "daisy_seed.h"
#include "main.h"
#include "core_cm7.h" //measuring
#include "stereo_buffer_chunk.h"
#include "sample.h"
#include "sp_ui.h"
#include "controls.h"
using namespace daisy;
using namespace daisysp;

//#define LOGG // start serial over USB Logger class
//#define MEASURE // measure MCU utilization

DaisySeed hardware;

float sysSampleRate;
float sysCallbackRate;

// sampler
#define BUFFER_MAX (48000 * 60) // 60 secs; 48k * 2 * 4 = 384k/s 
float DSY_SDRAM_BSS BufferR[BUFFER_MAX];
float DSY_SDRAM_BSS BufferL[BUFFER_MAX];
StereoBufferChunk Buffer(BufferL, BufferR, 48000, 2 * 48000);
int soundSeconds = 2;
StereoBufferChunk* soundBuffers[16];

Sample sample;

bool record;

Controls controls;
SPUI uiInstance;

enum millisecondDivisions : uint32_t{ // can be called without using millisecondDivisions.value. just value. Move scope into class or use 'enum class'
	one = 1000,
	two = 2000,
	three = 3000, //= 48 seconds so 12 seconds for loop section
	max = 3500
};

void AllocateBufferChunks(millisecondDivisions division){
	uint32_t start = 0;
	soundBuffers[0] = new StereoBufferChunk(BufferL, BufferR, 0, division * 48); // first chunk given start and end index of main buffer size
	start += division;
	for(int i=1; i<16; i++){
		soundBuffers[i] = new StereoBufferChunk(BufferL, BufferR, start * 48, (start + start) * 48);
		start += division;
	}
} 


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{	
	float sigR, sigL = 0.0f;
    
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
    
	controls.Init(&uiInstance);

   /*  Switch recButton;
    Switch playButton;
    recButton.Init(hardware.GetPin(29), 1000);
    playButton.Init(hardware.GetPin(30), 1000); */
	
	
	AllocateBufferChunks(two);
	sample.Init(sysSampleRate, soundBuffers[0]);

	System::Delay(100);

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
		#ifdef LOGG
		//PrintDebugInfo();
		#endif
		controls.UpdateControlStates();
		sample.RecordPrepare(uiInstance.recordStart);
		record = uiInstance.record;
        System::Delay(1);
    }
}
