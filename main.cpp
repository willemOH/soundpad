#include "daisy_seed.h"
#include "main.h"
#include "core_cm7.h" //measuring
#include "stereo_buffer_chunk.h"
#include "sample.h"
#include "sp_ui.h"
#include "controls.h"
#include "global.h"

//ostensibly necessary for sd card writing
#include "daisysp.h"
#include <stdio.h>
#include <string.h>
#include "daisy_core.h"
#include "util/wav_format.h"
#include "ff.h"
#include "fatfs.h"

using namespace daisy;
using namespace daisysp;

//#define MEASURE // measure MCU utilization

DaisySeed hardware;

SdmmcHandler sdcard;
FatFSInterface fsi;

float sysSampleRate;
float sysCallbackRate;

WavWriter<16384> writer;

// sampler
#define BUFFER_MAX (48000 * 349) // 349.52 secs; 48k * 2 (stereo) * 2  (16-bit or 2 bytes per sample) = 192k/s
float DSY_SDRAM_BSS Buffer[BUFFER_MAX];
//StereoBufferChunk StereoBuffer(Buffer, 48000, 2 * 48000);
int soundSeconds = 2;
StereoBufferChunk* soundBuffers[16];

float sigR = 0.0f;
float sigL = 0.0f;
Sample sample(sigL, sigR, writer); //tried to condense sample.init() into this constructor but no sound

Controls controls;
SPUI ui;



enum millisecondDivisions : uint32_t{ // can be called without using millisecondDivisions.value. just value. Move scope into class or use 'enum class'
	one = 1000,
	two = 2000,
	three = 3000, //= 48 seconds so 12 seconds for loop section
	max = 3500
};

void AllocateBufferChunks(millisecondDivisions division){
	uint32_t start = 0;
	soundBuffers[0] = new StereoBufferChunk(Buffer, 0, division * 48); // first chunk given start and end index of main buffer size
	start += division;
	for(int i=1; i<16; i++){
		soundBuffers[i] = new StereoBufferChunk(Buffer, start * 48, (start + start) * 48);
		start += division;
	}
} 


void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{	
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
		sample.Process();
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
    
	size_t blocksize = 4;

    sysSampleRate = hardware.AudioSampleRate();
	sysCallbackRate = hardware.AudioCallbackRate();
	
	AllocateBufferChunks(two);
	sample.Init(sysSampleRate, soundBuffers[0]);
	ui.Init(&sample);
	controls.Init(&ui);

	System::Delay(100);
	// logging over serial USB
	#ifdef LOGG
	hardware.StartLog(true); // start log but don't wait for PC - we can be connected to a battery
	hardware.PrintLine("monitoring started:");
	mLog(GENERAL, "general");
	mLog(BUTTON, "button");
	mLog(AUDIO, "audio");
	mLog(SD_CARD, "sd card");
	#endif
	
	//sGate = false;
	//sGatePrev = false;
	
	
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    if (sdcard.Init(sd_cfg) != SdmmcHandler::Result::OK)
    {
        mLog(SD_CARD,"SD card initialization failed");
    }
    if (fsi.Init(FatFSInterface::Config::MEDIA_SD) != FatFSInterface::Result::OK)
    {
		mLog(SD_CARD, "File system initialization failed");
    }
    if (f_mount(&fsi.GetSDFileSystem(), "/", 1) != FR_OK)
    {
        mLog(SD_CARD, "File system mount failed");
    }
    //sampler.Init(fsi.GetSDPath());
	System::Delay(100);

    WavWriter<16384>::Config config;
    config.samplerate = sysSampleRate; 
    config.channels = 2;          
    config.bitspersample = 16;    
    writer.Init(config);


	// start callback
	hardware.SetAudioBlockSize(blocksize);
    hardware.StartAudio(AudioCallback);

    //update loop
    for(;;)
    { 
		controls.UpdateControlStates();
		sample.WriteProcess();
        System::Delay(1);
    }
}
