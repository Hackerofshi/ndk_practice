//
// Created by shixin on 2022/10/14.
//

#include "audio_decoder.h"


AudioDecoder::AudioDecoder(JNIEnv *env,
                           const jstring path,
                           bool forSynthesizer)
        : BaseDecoder(env, path, forSynthesizer) {
}

AudioDecoder::~AudioDecoder() {
    delete m_render;
}

void AudioDecoder::SetRender(AudioRender *render) {
    m_render = render;
}

void AudioDecoder::Prepare(JNIEnv *env) {
    InitSwr();
    CalculateSampleArgs();
    InitOutBuffer();
    InitRender();
}

void AudioDecoder::InitSwr() {
    AVCodecContext *codecCtx = codec_cxt();

    LOGI("-------------------音频解码配置开始")
    m_swr = swr_alloc();
    av_opt_set_int(m_swr, "in_channel_layout", codecCtx->channel_layout, 0);
    av_opt_set_int(m_swr, "out_channel_layout", ENCODE_AUDIO_DEST_CHANNEL_LAYOUT, 0);
    av_opt_set_int(m_swr, "in_sample_rate", codecCtx->sample_rate, 0);
    av_opt_set_int(m_swr, "out_sample_rate", GetSampleRate(codecCtx->sample_rate), 0);
    av_opt_set_int(m_swr, "in_sample_fmt", codecCtx->sample_fmt, 0);
    av_opt_set_int(m_swr, "out_sample_fmt", GetSampleFmt(), 0);

    swr_init(m_swr);
    LOGI("-------------------音频解码配置结束")

    LOGI(TAG, "sample rate: %d, channel: %d, format: %d, frame_size: %d, layout: %lld",
         codecCtx->sample_rate, codecCtx->channels, codecCtx->sample_fmt, codecCtx->frame_size,
         codecCtx->channel_layout)
}

void AudioDecoder::CalculateSampleArgs() {
    LOGI("-------------------音频解码计算开始")
    //重采样后一个通道的采样数
    m_dest_nb_sample = (int) av_rescale_rnd(ACC_NB_SAMPLES, GetSampleRate(codec_cxt()->sample_rate),
                                            codec_cxt()->sample_rate, AV_ROUND_UP);

    //重采样后一帧数据的大小
    m_dest_data_size = (size_t) av_samples_get_buffer_size(nullptr,
                                                           ENCODE_AUDIO_DEST_CHANNEL_COUNTS,
                                                           m_dest_nb_sample, GetSampleFmt(), 1);
    LOGI("-------------------音频解码计算结束")

}

void AudioDecoder::InitOutBuffer() {
    if (ForSynthesizer()) {
        m_out_buffer[0] = (uint8_t *) malloc(m_dest_data_size / 2);
        m_out_buffer[1] = (uint8_t *) malloc(m_dest_data_size / 2);
    } else {
        m_out_buffer[0] = (uint8_t *) malloc(m_dest_data_size);
    }
}

void AudioDecoder::InitRender() {
    if (m_render != nullptr) {
        LOGI("-------------------渲染开始")
        m_render->InitRender();
        LOGI("-------------------渲染结束")
    }
}

void AudioDecoder::Render(AVFrame *frame) {
    InitOutBuffer();

    //转换返回每个通道的样本数
    int ret = swr_convert(m_swr, m_out_buffer, m_dest_data_size / 2, (const uint8_t **) frame->data,
                          frame->nb_samples);

    LOGI("-------------------渲染结束%d", ret)

    if (ret > 0) {
        if (ForSynthesizer()) {
            /*if (m_state_cb != nullptr) {
                auto *one_frame = new OneFrame(m_out_buffer[0], m_dest_data_size, frame->pts,
                                               time_base(), m_out_buffer[1], true);
                if (m_state_cb->DecodeOneFrame(this, one_frame)) {
                    Wait(0, 200);
                }
            }*/
        } else {
            m_render->Render(m_out_buffer[0], (size_t) m_dest_data_size);
        }
    }

}

void AudioDecoder::Release() {
    if (m_swr != nullptr) {
        swr_free(&m_swr);
    }

    ReleaseOutBuffer();
}

void AudioDecoder::ReleaseOutBuffer() {
    if (m_out_buffer[0] != nullptr) {
        free(m_out_buffer[0]);
        m_out_buffer[0] = nullptr;
    }
}