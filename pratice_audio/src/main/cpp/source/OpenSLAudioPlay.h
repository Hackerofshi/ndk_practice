//
// Created by shixin on 2022/10/11.
//

#ifndef NDK_PRACTICE_OPENSLAUDIOPLAY_H
#define NDK_PRACTICE_OPENSLAUDIOPLAY_H


#include "AudioEngine.h"
#include <pthread.h>
#include <jni.h>

#define SAMPLE_FORMAT_16  16


class OpenSLAudioPlay {
private:
    AudioEngine *mAudioEngine;
    SLObjectItf mPlayerObj;
    SLPlayItf mPlayer;
    SLAndroidSimpleBufferQueueItf mBufferQueue;
    SLEffectSendItf mEffectSend;
    SLVolumeItf mVolume;
    SLmilliHertz mSampleRate;
    int mSampleFormat;
    int mChannels;

    uint8_t *mBuffers[2];
    SLuint32 mBufferSize;
    int mIndex;
    pthread_mutex_t mMutex;

public:
    OpenSLAudioPlay(int sampleRate, int sampleFormat, int channels);

    bool init();

    void enqueueSample(void *data, size_t length);

    void release();

    ~OpenSLAudioPlay();

    friend void playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

};


#endif //NDK_PRACTICE_OPENSLAUDIOPLAY_H
