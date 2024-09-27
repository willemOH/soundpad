// i_sample.h
#ifndef I_SAMPLE_H
#define I_SAMPLE_H

class ISample {
public:
    virtual void RecordPrepare();
    virtual void SetRecord(bool recordState);
    virtual void SetPlayback(bool playState);
    virtual void Record();
    virtual void Playback();
};

#endif // I_SAMPLE_H