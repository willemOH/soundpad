#include <stdio.h>
#include <string.h>
#include "sys/fatfs.h"

//Here is the working Class for anyone looking at this thread. Currently works for 16bit PCM at 48KHz. 
//The card must be initialized using f_mount before calling SetSample.

class SampleVoice {
private:
	uint32_t bufferLength;
	size_t length;
	size_t position;
	bool playing;
	int16_t *buffer;
	FIL fp;
	bool stereo;
public:
	
	// buffer should be predefined in SDRAM
	void Init(int16_t *buffer, uint32_t bufferLength) {
		this->buffer = buffer;
		this->bufferLength = bufferLength;
		length = 0;
		position = 0;
		playing = false;
	}

	// starts sample playing. need to add handling of sample already playing.
	void Play() {
		playing = true;
	}


	float Stream() {
		if (playing) {
			if (position >= length) {
				playing = false;
				position = 0;
				return 0.0;
			}
			return s162f(buffer[position++]);
		}
		return 0.0;
	}

    inline float s162f(int16_t sample) {
    // Convert 16-bit signed integer to float in the range [-1.0, 1.0]
    return sample / 32768.0f;
    }

	void SetLength(size_t length) {this->length = length;}

	void *GetBuffer() {return (void *)buffer;}

	uint32_t GetBufferLength() {return bufferLength;}

	bool IsStereo() {return stereo;}
	bool IsMono() {return !stereo;}

	/* adds given file to the buffer. Only supports 16bit PCM 48kHz. 
	 * If Stereo samples are interleaved left then right.
	 * return 0: succesful, 1: file read failed, 2: invalid format, 3: file too large
	*/ 
	int SetSample(TCHAR *fname) {
		UINT bytesread;
		WAV_FormatTypeDef wav_data; 
		
		memset(buffer, 0, bufferLength);
		
		if(f_open(&fp, fname, (FA_OPEN_EXISTING | FA_READ)) == FR_OK) {
			// Populate the WAV Info
			if(f_read(&fp, (void *)&wav_data, sizeof(WAV_FormatTypeDef), &bytesread) != FR_OK) return 1;	
		} else return 1;

		if (wav_data.SampleRate != 48000 || wav_data.BitPerSample != 16) return 2;
		if (wav_data.SubCHunk2Size > bufferLength || wav_data.NbrChannels > 2) return 3;
		stereo = wav_data.NbrChannels == 2;

		if (f_lseek(&fp, sizeof(WAV_FormatTypeDef)) != FR_OK) return 1;
		if(f_read(&fp, buffer, wav_data.SubCHunk2Size, &bytesread) != FR_OK)return 1;
		length = bytesread / (wav_data.BitPerSample/8);

		f_close(&fp);
		return 0;
	}
};