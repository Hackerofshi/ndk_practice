//
// Created by shixin on 2022/10/14.
//

#include "opensl_render.h"

OpenSLRender::OpenSLRender() {}

OpenSLRender::~OpenSLRender() {}

void OpenSLRender::InitRender() {
    if (!CreateEngine()) {
        return;
    }
    if (!CreateOutputMixer()) return;
    if (!ConfigPlayer()) return;

    std::thread t(sRenderPcm, this);
    t.detach();
}

void OpenSLRender::Render(uint8_t *pcm, int size) {
    if (m_pcm_player) {
        if (pcm != nullptr && size > 0) {
            while (m_data_queue.size() >= 2) {
                SendCacheReadySignal();
                usleep(20000);
            }
            //将数据复制一份并压入队列
            auto *data = (uint8_t *) malloc(size);
            memcpy(data, pcm, size);

            auto *pcmData = new PcmData(data, size);
            m_data_queue.push(pcmData);

            //通知到播放线程退出等待，恢复播放
            SendCacheReadySignal();
        }
    } else {
        free(pcm);
    }
}

void OpenSLRender::ReleaseRender() {
    //设置停止状态
    if (m_pcm_player) {
        (*m_pcm_player)->SetPlayState(m_pcm_player, SL_PLAYSTATE_STOPPED);
        m_pcm_player = nullptr;
    }
    //先通知回调接口结束，否则可能导致无法销毁：m_pcm_player_obj
    SendCacheReadySignal();

    //销毁播放器
    if (m_pcm_player_obj) {
        (*m_pcm_player_obj)->Destroy(m_output_mix_obj);
        m_output_mix_obj = nullptr;
    }

    //销毁混音器
    if (m_output_mix_obj) {
        (*m_output_mix_obj)->Destroy(m_output_mix_obj);
        m_output_mix_obj = nullptr;
    }

    //销毁引擎
    if (m_engine_obj) {
        (*m_engine_obj)->Destroy(m_engine_obj);
        m_engine_obj = nullptr;
        m_engine = nullptr;
    }

    for (int i = 0; i < m_data_queue.size(); ++i) {
        PcmData *pcm = m_data_queue.front();
        m_data_queue.pop();
        delete pcm;
    }
}

bool OpenSLRender::CreateEngine() {
    SLresult result = slCreateEngine(&m_engine_obj, 0,
                                     nullptr, 0,
                                     nullptr, nullptr);
    if (checkError(result, "eigine")) return false;

    result = (*m_engine_obj)->Realize(m_engine_obj, SL_BOOLEAN_FALSE);
    if (checkError(result, "eigine realize")) return false;

    result = (*m_engine_obj)->GetInterface(m_engine_obj, SL_IID_ENGINE, &m_engine);

    return !checkError(result, "Engine Interface");
}

bool OpenSLRender::CreateOutputMixer() {
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    SLresult result = (*m_engine)->CreateOutputMix(m_engine, &m_output_mix_obj, 1, mids, mreq);
    if (checkError(result, "output mix")) return false;

    result = (*m_output_mix_obj)->Realize(m_output_mix_obj, SL_BOOLEAN_FALSE);
    if (checkError(result, "output mix Realize")) return false;

    result = (*m_output_mix_obj)->GetInterface(m_output_mix_obj, SL_IID_ENVIRONMENTALREVERB,
                                               &m_output_mix_evn_reverb);
    if (checkError(result, "output Mix Env Reverb")) return false;

    if (result != SL_RESULT_SUCCESS) {
        (*m_output_mix_evn_reverb)->SetEnvironmentalReverbProperties(m_output_mix_evn_reverb,
                                                                     &m_reverb_setting);
    }
    return true;
}

bool OpenSLRender::ConfigPlayer() {
    SLDataLocator_AndroidBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                      SL_QUEUE_BUFFER_COUNT};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM, //播放pcm格式的数据
            (SLuint32) 2,//2个声道
            SL_SAMPLINGRATE_44_1,//44100hz的频率
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,///位数 16位
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT //立体声 前左，前右
            , SL_BYTEORDER_LITTLEENDIAN
    };

    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_output_mix_obj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,};


    SLresult result = (*m_engine)->CreateAudioPlayer(m_engine, &m_pcm_player_obj, &slDataSource,
                                                     &slDataSink, 3, ids, req);
    if (checkError(result, "Player")) return false;

    //初始化播放器
    result = (*m_pcm_player_obj)->Realize(m_pcm_player_obj, SL_BOOLEAN_FALSE);
    if (checkError(result, "Player Realize")) return false;


    //得到接口后调用，获取Player接口
    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_PLAY, &m_pcm_player);
    if (checkError(result, "Player Interface")) return false;

    //注册回调缓冲区，获取缓冲队列接口
    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_BUFFERQUEUE, &m_pcm_buffer);
    if (checkError(result, "Player  Queue Buffer")) return false;

    //缓冲接口回调
    result = (*m_pcm_buffer)->RegisterCallback(m_pcm_buffer, sReadPcmBufferCBFun, this);
    if (checkError(result, "Register Callback Interface")) return false;

    //获取音量接口
    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_VOLUME,
                                               &m_pcm_player_volume);
    if (checkError(result, "Register Volume Interface ")) return false;


    LOGI("OpenSL ES init success");

    return true;

}


bool OpenSLRender::checkError(SLresult result, std::string hint) {
    if (SL_RESULT_SUCCESS != result) {
        LOGE("OpenSL ES [%s] init fail", hint.c_str())
        return true;
    }
    return false;
}

void OpenSLRender::sRenderPcm(OpenSLRender *that) {
    that->StartRender();
}


void OpenSLRender::StartRender() {
    while (m_data_queue.empty()) {
        waitForCache();
    }

    (*m_pcm_player)->SetPlayState(m_pcm_player, SL_PLAYSTATE_PLAYING);
    sReadPcmBufferCBFun(m_pcm_buffer, this);
    LOGI("openSL render start playing")
}

void OpenSLRender::sReadPcmBufferCBFun(SLAndroidSimpleBufferQueueItf bufferQueueItf,
                                       void *context) {
    auto *player = (OpenSLRender *) context;
    player->BolckEnqueue();
}

void OpenSLRender::BolckEnqueue() {
    if (m_pcm_player == nullptr) return;

    //先将已经使用过的数据移除
    while (!m_data_queue.empty()) {
        PcmData *pcm = m_data_queue.front();
        if (pcm->used) {
            m_data_queue.pop();
            delete pcm;
        } else {
            break;
        }
    }

    while (m_data_queue.empty() && m_pcm_player != nullptr) {// if m_pcm_player is NULL, stop render
        waitForCache();
    }

    PcmData *pcmData = m_data_queue.front();
    if (nullptr != pcmData && m_pcm_player) {
        SLresult lresult = (*m_pcm_buffer)->Enqueue(m_pcm_buffer, pcmData->pcm,
                                                    (SLuint32) pcmData->size);
        if (lresult == SL_RESULT_SUCCESS) {
            // 只做已经使用标记，在下一帧数据压入前移除
            // 保证数据能正常使用，否则可能会出现破音
            pcmData->used = true;
        }
    }
}
