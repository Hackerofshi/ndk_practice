//
// Created by shixin on 2022/10/11.
//

#ifndef NDK_PRACTICE_OPENSLRECORD_H
#define NDK_PRACTICE_OPENSLRECORD_H

#include "AudioEngine.h"

#define  NUM_RECORDER_EXPLICIT_INTERFACES 2

class OpenSLRecord {

private:
    AudioEngine *mAudioEngine;
    SLObjectItf recorderObject;
    SLAndroidSimpleBufferQueueItf recorderBuffQueueItf;


public:
    OpenSLRecord(FILE *);

    bool init();

    void startRecord();

    void stopRecord();

    void release();

    uint8_t *mBuffers;
    SLuint32 mBufferSize;
    SLRecordItf recorderRecord;
    FILE *pcmFile;
};


#endif //NDK_PRACTICE_OPENSLRECORD_H
