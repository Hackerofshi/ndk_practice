//
// Created by shixin on 2022/10/11.
//

#ifndef NDK_PRACTICE_AUDIOENGINE_H
#define NDK_PRACTICE_AUDIOENGINE_H

#include <SLES/OpenSLES.h>
#include "stdio.h"
#include "SLES/OpenSLES_Android.h"
#include "assert.h"
#include "android/log.h"


class AudioEngine {
public:
    SLObjectItf engineObj;
    SLEngineItf engine;

    SLObjectItf outPutMixObj;
private:
    void createEngine() {
        SLEngineOption pEngineOptions[] = {(SLuint32) SL_ENGINEOPTION_THREADSAFE,
                                           (SLuint32) SL_BOOLEAN_TRUE};


        // 音频的播放，就涉及到了，OpenLSES
        // TODO 第一大步：创建引擎并获取引擎接口
        // 1.1创建引擎对象：SLObjectItf engineObject
        SLresult result = slCreateEngine(&engineObj,//对象地址，用于传出对象
                                         1,//配置参数数量
                                         pEngineOptions,//配置参数，为枚举数组
                                         0, //支持接口的数量
                                         nullptr, //具体要支持的接口
                                         nullptr // 具体的要支持的接口是开放的还是关闭的，也是一个数组，这三个参数长度是一致的

        );
        if (SL_RESULT_SUCCESS != result) {
            return;
        }


        //1.2 初始化引擎
        result = (*engineObj)->Realize(engineObj, SL_BOOLEAN_FALSE);
        if (SL_BOOLEAN_FALSE != result) {
            return;
        }

        //1.3 获取引擎接口
        result = (*engineObj)->GetInterface(engineObj, SL_IID_ENGINE, &engine);
        if (SL_RESULT_SUCCESS != result) {
            return;
        }


        // TODO 第二大步 设置混音器
        result = (*engine)->CreateOutputMix(engine, &outPutMixObj, 0, 0, 0);

        if (SL_RESULT_SUCCESS != result) {
            return;
        }

        result = (*outPutMixObj)->Realize(outPutMixObj, SL_BOOLEAN_FALSE);
        if (SL_BOOLEAN_FALSE != result) {
            return;
        }
    }

    virtual void release() {
        if (outPutMixObj) {
            if (outPutMixObj) {
                (*outPutMixObj)->Destroy(outPutMixObj);
                outPutMixObj = nullptr;
            }
        }
        if (engineObj) {
            (*engineObj)->Destroy(engineObj);
            engineObj = nullptr;
            engine = nullptr;
        }
    }

public:
    AudioEngine() : engineObj(nullptr), engine(nullptr), outPutMixObj(nullptr) {
        createEngine();
    }

    virtual ~AudioEngine() {
        release();
    }

};


#endif //NDK_PRACTICE_AUDIOENGINE_H
