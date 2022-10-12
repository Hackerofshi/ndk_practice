//
// Created by shixin on 2022/10/11.
//

#ifndef NDK_PRACTICE_I_DECODER_H
#define NDK_PRACTICE_I_DECODER_H

//这是一个纯虚类，类似 Java 的 interface
class IDecoder {
public:
    virtual void GoOn() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() = 0;
    virtual long GetDuration() = 0;
    virtual long GetCurPos() = 0;
};

#endif //NDK_PRACTICE_I_DECODER_H
