#include "playback.h"

void Playback::ProcessAudioFrame(){
    if (index < end)
        {
            factor = settings.speed * (freq / 440.0f);
            /* 	factor = settings.speed * (freq / 440.0f) * (1.0f + std::pow(2.0f, settings.tune / 1200.0f)); */
            float sigL = sBuffer->getSample(index).left;
            float sigR = sBuffer->getSample(index).right;
			//index += 1.0;
      		index += factor;		
            //index += factor;
			/* if (loop){
				if (index >= settings.loopEnd)
				{
					index = settings.loopStart;
				}
			}; */
            output.left=sigL;
            output.right=sigR;
        }
        else if(loop){
			index = start;
        }
		else{ //making this else if(!loop) caused weird play button holding glitches
			play = false;
		}
}

void Playback::SetNote(float slider){
	 static auto hasNoteChanged = [](float newNoteNum) { //temp code to hear note change better
        static float lastNoteNum = -1.0f; 
        if (newNoteNum != lastNoteNum) {
            lastNoteNum = newNoteNum;
            return true;
        }
        return false;
    };
	int scaledValue = static_cast<int>(std::floor(slider * 8));
	float noteNum = scaledValue % 8;

	if (hasNoteChanged(noteNum)) {
	freq = daisysp::mtof(noteNum + (12.f * 6));
	if(!settings.loop){
        index = settings.startSaved;
        factor = settings.speed * (freq / 440.0f);
        play = true;
	}
	}
}

void Playback::PlayPrepare(){
    index = settings.startSaved;
    start = settings.startSaved;
    end = settings.endSaved;
    loop = settings.loop;
}
