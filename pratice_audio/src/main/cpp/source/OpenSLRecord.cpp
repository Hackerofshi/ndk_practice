//
// Created by shixin on 2022/10/11.
//

#include "OpenSLRecord.h"


OpenSLRecord::OpenSLRecord(FILE *file) : mAudioEngine(new AudioEngine()),
                                         recorderObject(nullptr),
                                         recorderRecord(nullptr),
                                         recorderBuffQueueItf(nullptr),
                                         mBufferSize(1024) {
    mBuffers = new uint8_t[1024]; //数据缓存区，

    /*    mBuffers[0] = nullptr;
        mBuffers[1] = nullptr;*/
    pcmFile = file;
}


void AudioRecorderCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    //注意这个是另外一条采集线程回调
    OpenSLRecord *recorderContext = (OpenSLRecord *) context;
    assert(recorderContext != NULL);
    if (recorderContext->mBuffers != NULL) {
        fwrite(recorderContext->mBuffers, recorderContext->mBufferSize, 1,
               recorderContext->pcmFile);
        //LOGD("save a frame audio data,pid=%ld", syscall(SYS_gettid));
        SLresult result;
        SLuint32 state;
        result = (*(recorderContext->recorderRecord))->GetRecordState(
                recorderContext->recorderRecord, &state);
        assert(SL_RESULT_SUCCESS == result);
        (void) result;
        if (state == SL_RECORDSTATE_RECORDING) {
            //取完数据，需要调用Enqueue触发下一次数据回调
            result = (*bufferQueueItf)->Enqueue(bufferQueueItf, recorderContext->mBuffers,
                                                recorderContext->mBufferSize);
            assert(SL_RESULT_SUCCESS == result);
            (void) result;
        }
    }
}

bool OpenSLRecord::init() {
    SLresult result;

    SLDataLocator_IODevice ioDevice = {
            SL_DATALOCATOR_IODEVICE,  //类型 这里只能是SL_DATALOCATOR_IODEVICE
            SL_IODEVICE_AUDIOINPUT,//device类型  选择了音频输入类型
            SL_DEFAULTDEVICEID_AUDIOINPUT, //deviceID 对应的是SL_DEFAULTDEVICEID_AUDIOINPUT
            NULL//device实例
    };
    // 输入，SLDataSource 表示音频数据来源的信息
    SLDataSource recSource = {
            &ioDevice,//SLDataLocator_IODevice配置输入
            NULL//输入格式，采集的并不需要
    };
    // 数据源简单缓冲队列定位器,输出buffer队列
    SLDataLocator_AndroidSimpleBufferQueue recBufferQueue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, //类型 这里只能是SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE
            2 //buffer的数量
    };
    // PCM 数据源格式 //设置输出数据的格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM, //输出PCM格式的数据
            2,  //  //输出的声道数量2 个声道（立体声）
            SL_SAMPLINGRATE_44_1, //输出的采样频率，这里是44100Hz
            SL_PCMSAMPLEFORMAT_FIXED_16, //输出的采样格式，这里是16bit
            SL_PCMSAMPLEFORMAT_FIXED_16,//一般来说，跟随上一个参数
            SL_SPEAKER_FRONT_LEFT |
            SL_SPEAKER_FRONT_RIGHT,//双声道配置，如果单声道可以用 SL_SPEAKER_FRONT_CENTER
            SL_BYTEORDER_LITTLEENDIAN //PCM数据的大小端排列
    };
    // 输出，SLDataSink 表示音频数据输出信息
    SLDataSink dataSink = {
            &recBufferQueue, //SLDataFormat_PCM配置输出
            &pcm //输出数据格式
    };


    //创建录制的对象，并且指定开放SL_IID_ANDROIDSIMPLEBUFFERQUEUE这个接口
    SLInterfaceID iids[NUM_RECORDER_EXPLICIT_INTERFACES] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                                            SL_IID_ANDROIDCONFIGURATION};
    SLboolean required[NUM_RECORDER_EXPLICIT_INTERFACES] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    /* Create the audio recorder */
    // 创建 audio recorder 对象
    result = (*mAudioEngine->engine)->CreateAudioRecorder(mAudioEngine->engine, //引擎接口
                                                          &recorderObject, //录制对象地址，用于传出对象
                                                          &recSource,//输入配置
                                                          &dataSink,//输出配置
                                                          NUM_RECORDER_EXPLICIT_INTERFACES,//支持的接口数量
                                                          iids, //具体的要支持的接口
                                                          required //具体的要支持的接口是开放的还是关闭的
    );
    assert(SL_RESULT_SUCCESS == result);

    /* Realize the recorder in synchronous mode. */ //实例化这个录制对象
    result = (*recorderObject)->Realize(recorderObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);


    /* Get the buffer queue interface which was explicitly requested *///获取Buffer接口
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE,
                                             (void *) &recorderBuffQueueItf);
    assert(SL_RESULT_SUCCESS == result);

    /* get the record interface */ //获取录制接口
    result = (*recorderObject)->GetInterface(recorderObject, SL_IID_RECORD, &recorderRecord);
    assert(SL_RESULT_SUCCESS == result);



    //设置数据回调接口AudioRecorderCallback，最后一个参数是可以传输自定义的上下文引用
    result = (*recorderBuffQueueItf)->RegisterCallback(recorderBuffQueueItf, AudioRecorderCallback,
                                                       this);
    assert(SL_RESULT_SUCCESS == result);

    return true;
}

void OpenSLRecord::startRecord() {
    SLresult result;


    /* Start recording */
    // 开始录制音频，//设置录制器为录制状态 SL_RECORDSTATE_RECORDING
    result = (*recorderRecord)->SetRecordState(recorderRecord, SL_RECORDSTATE_RECORDING);
    assert(SL_RESULT_SUCCESS == result);

    // 在设置完录制状态后一定需要先Enqueue一次，这样的话才会开始采集回调
    /* Enqueue buffers to map the region of memory allocated to store the recorded data */
    result = (*recorderBuffQueueItf)->Enqueue(recorderBuffQueueItf, mBuffers, mBufferSize);
    assert(SL_RESULT_SUCCESS == result);
    //LOGD("Starting recording tid=%ld", syscall(SYS_gettid));//线程id
}

void OpenSLRecord::stopRecord() {
    SLresult result;
    // 停止录制
    if (recorderRecord != nullptr) {
        //设置录制器为停止状态 SL_RECORDSTATE_STOPPED
        result = (*recorderRecord)->SetRecordState(recorderRecord,
                                                   SL_RECORDSTATE_STOPPED);
        assert(SL_RESULT_SUCCESS == result);
        fclose(pcmFile);
        pcmFile = nullptr;

        delete mBuffers;
    }
}

void OpenSLRecord::release() {
    //只需要销毁OpenSL ES对象，接口不需要做Destroy处理。
    if (recorderObject != nullptr) {
        (*recorderObject)->Destroy(recorderObject);
        recorderObject = nullptr;
        recorderRecord = nullptr;
        recorderBuffQueueItf = nullptr;
        delete mBuffers;
    }

    if (mAudioEngine) {
        delete mAudioEngine;
        mAudioEngine = nullptr;
    }
}
