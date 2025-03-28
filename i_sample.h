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
    virtual void SetStart(float fraction);
    virtual void SetEnd(float fraction);
    virtual bool GetLoopState();
    virtual void SetSpeed(float ratio);
    virtual void SetNote(float noteNum);
};

#endif // I_SAMPLE_H