// i_sample.h
#ifndef I_SAMPLE_H
#define I_SAMPLE_H

class ISample {
public:
    virtual void RecordPrepare();
    virtual void PlayPrepare();
    virtual void SetRecord(bool recordState);
    virtual void SetPlayback(bool playState);
    virtual void SetLoop(bool loopState);
    virtual void Record();
    virtual void Playback();
    virtual void SetStart(float fraction);
    virtual void SetEnd(float fraction);
    virtual bool GetLoopState();
};

#endif // I_SAMPLE_H