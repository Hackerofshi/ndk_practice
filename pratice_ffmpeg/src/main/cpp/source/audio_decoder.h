//
// Created by shixin on 2022/10/14.
//

#ifndef NDK_PRACTICE_AUDIO_DECODER_H
#define NDK_PRACTICE_AUDIO_DECODER_H

#include "base_decoder.h"
#include "const.h"
#include "../render/audio/audio_render.h"
#include "test/base_decoder1.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/opt.h>

}

class AudioDecoder : public BaseDecoder {

private:
    const char *TAG = "AUDIO_DECODER";
    //音频转换器
    //其中，SwrContext 是 FFmpeg 提供的音频转化工具，位于 swresample 中，可用来转换采样率、解码通道数、
    // 采样位数等。这里用来将音频数据转换为 双通道立体 声音，统一 采样位数。
    SwrContext *m_swr = nullptr;

    //音频渲染器
    AudioRender *m_render = nullptr;

    //输出缓冲
    uint8_t *m_out_buffer[2] = {nullptr, nullptr};

    //重采样后，每个通道包含的采样数
    //acc默认为1024，重采样后可能变化
    int m_dest_nb_sample = 1024;

    //重采样后一帧数据的大小
    size_t m_dest_data_size = 0;

    //初始化转换工具
    void InitSwr();

    /**
     * 计算重采样后通道采样数和帧数据大小
     */
    void CalculateSampleArgs();


    //初始化输出缓冲
    void InitOutBuffer();

    /**
     * 初始化渲染器
     */
    void InitRender();


    //释放缓冲区
    void ReleaseOutBuffer();

    /**
     * 采样格式：16位
     */
    AVSampleFormat GetSampleFmt() {
        if (ForSynthesizer()) {
            return ENCODE_AUDIO_DEST_FORMAT;
        } else {
            return AV_SAMPLE_FMT_S16;
        }
    }

    /**
     * 采样率
     */
    int GetSampleRate(int spr) {
        if (ForSynthesizer()) {
            return ENCODE_AUDIO_DEST_SAMPLE_RATE;
        } else {
            return spr;
        }
    }

public:
    AudioDecoder(JNIEnv *env, const jstring path, bool forSynthesizer);

    ~AudioDecoder();

    void SetRender(AudioRender *render);

protected:
    void Prepare(JNIEnv *env) override;

    void Render(AVFrame *frame) override;

    void Release() override;


    bool NeedLoopDecode() override {
        return true;
    }

    AVMediaType GetMediaType() override {
        return AVMEDIA_TYPE_AUDIO;
    }

    const char *const LogSpec() override {
        return "AUDIO";
    };

};


#endif //NDK_PRACTICE_AUDIO_DECODER_H
