//
// Created by shixin on 2022/10/11.
//

#include "base_decoder.h"
#include "../../utils/timer.c"

BaseDecoder::BaseDecoder(JNIEnv *env, jstring path, bool for_synthesizer)
        : m_for_synthesizer(for_synthesizer) {
    Init(env, path);
    CreateDecodeThread();
}


BaseDecoder::~BaseDecoder() {
    if (m_format_ctx != nullptr) delete m_format_ctx;
    if (m_codec_ctx != NULL) delete m_codec_ctx;
    if (m_frame != NULL) delete m_frame;
    if (m_packet != NULL) delete m_packet;
}


void BaseDecoder::Init(JNIEnv *env, jstring path) {

    m_path_ref = env->NewGlobalRef(path);
    m_path = env->GetStringUTFChars(path, nullptr);
    //获取jvm虚拟机，为创建线程准备
    env->GetJavaVM(&m_jvm_for_thread);
}

//说明：由于 JNIEnv 和 线程 是一一对应的，也就是说，在 Android 中
// JNI环境 是和线程绑定的，每一个线程都有一个独立的 JNIEnv 环境，并且互相之间不可访问。
// 所以如果要在新的线程中访问 JNIEnv，需要为这个线程创建一个新的 JNIEnv 。
//
//在 Init 方法的最后，通过 env->GetJavaVM(&m_jvm_for_thread) 获取到 JavaVM 实例，
// 保存到 m_jvm_for_thread，该实例是所有共享的 ，通过它就可以为解码线程获取一个新的 JNIEnv 环境。
//
void BaseDecoder::CreateDecodeThread() {
    //使用智能指针，线程结束时，自动删除本类指针
    std::shared_ptr<BaseDecoder> that(this);

    //在 C++ 中创建线程非常简单，只需两句话，就可以启动一个线程：
    //
    //std::thread t(静态方法, 静态方法参数);
    //t.detach();

    std::thread t(Decode, that);
    t.detach();
}

void BaseDecoder::Decode(std::shared_ptr<BaseDecoder> that) {
    JNIEnv *env;
    if (that->m_jvm_for_thread->AttachCurrentThread(&env, NULL) != JNI_OK) {
        LOGE("Fail to Init decode thread");
        return;
    }

    //初始化解码器
    that->InitFFMpegDecoder(env);
    //分配解码桢数据内存
    that->AllocFrameBuffer();
    //回调子类方法，通知子类解码器初始化完毕
    that->Prepare(env);
    //进入解码循环
    that->loopDecode();
    //退出解码
    that->DoneDecode(env);

    //解码线程和jvm关联
    that->m_jvm_for_thread->DetachCurrentThread();
}

void BaseDecoder::InitFFMpegDecoder(JNIEnv *env) {
    //初始化上下文
    m_format_ctx = avformat_alloc_context();

    //打开文件
    if (avformat_open_input(&m_format_ctx, m_path, nullptr, nullptr) != 0) {
        LOGE("Fail to open file [%s]", m_path);
        DoneDecode(env);
        return;
    }

    //获取音视频流信息
    if (avformat_find_stream_info(m_format_ctx, nullptr) < 0) {
        LOGE("fail to find stream info");
        DoneDecode(env);
        return;
    }

    //4查找编解码器
    //4.1获取视频流索引
    int vIdx = -1;//存放视频流索引
    for (int i = 0; i < m_format_ctx->nb_streams; ++i) {
        if (m_format_ctx->streams[i]->codecpar->codec_type == GetMediaType()) {
            vIdx = i;
            break;
        }
    }

    if (vIdx == -1) {
        LOGE("fail to find stream index");
        DoneDecode(env);
        return;
    }

    m_stream_index = vIdx;

    //4.2 获取解码器参数
    AVCodecParameters *codecPar = m_format_ctx->streams[vIdx]->codecpar;

    //4.3 获取解码器
    m_codec = avcodec_find_decoder(codecPar->codec_id);

    //4.4获取解码器上下文
    m_codec_ctx = avcodec_alloc_context3(m_codec);

    if (avcodec_parameters_to_context(m_codec_ctx, codecPar) != 0) {
        LOGE("fail to obtain av codec context");
        DoneDecode(env);
        return;
    }

    //打开解码器
    if (avcodec_open2(m_codec_ctx, m_codec, nullptr) < 0) {
        LOGE("fail to open av codec");
        DoneDecode(env);
        return;
    }

    m_duration = (long) ((float) m_format_ctx->duration / AV_TIME_BASE * 1000);

    LOGI(TAG, "Decoder init success");
}


void BaseDecoder::AllocFrameBuffer() {
    //初始化待解码和解码数据结构
    // 初始化AVPacket ，存放解码前数据
    m_packet = av_packet_alloc();
    //初始化av_frame 存放解码后数据
    m_frame = av_frame_alloc();
}

void BaseDecoder::loopDecode() {
    if (STOP == m_state) {
        m_state = START;
    }
    LOGI(TAG, "Start loop decode");
    while (1) {
        if (m_state != DECODING && m_state != START && m_state != STOP) {
            Wait();
            //恢复同步起始时间，去除等待流失时间
            m_started_t = GetCurMsTime() - m_cur_t_s;
        }
        if (m_state == STOP) {
            break;
        }
        if (-1 == m_started_t) {
            m_started_t = GetCurMsTime();
        }
        if (DecodeOneFrame() != NULL) {
            SyncRender();
            Render(m_frame);
            if (m_state == START) {
                m_state = PAUSE;
            }
        } else {
            LOGI("m_state = %d", m_state);
            if (ForSynthesizer()) {
                m_state = STOP;
            } else {
                m_state = FINISH;
            }
        }
    }
}

AVFrame *BaseDecoder::DecodeOneFrame() {
    int ret = av_read_frame(m_format_ctx, m_packet);
    while (ret == 0) {
        if (m_packet->stream_index == m_stream_index) {
            //将数据放入m_packet中
            switch (avcodec_send_packet(m_codec_ctx, m_packet)) {
                case AVERROR_EOF: {
                    av_packet_unref(m_packet);
                    LOGE("Decode error: %s", av_err2str(AVERROR_EOF));
                    return NULL;//解码结束
                }
                case AVERROR(EAGAIN):
                    LOGE("Decode error: %s", av_err2str(EAGAIN));
                    break;
                case AVERROR(EINVAL):
                    LOGE("Decode error: %s", av_err2str(EINVAL));
                    break;
                case AVERROR(ENOMEM):
                    LOGE("Decode error: %s", av_err2str(ENOMEM));
                    break;
                default:
                    break;
            }
            int result = avcodec_receive_frame(m_codec_ctx, m_frame);
            if (result == 0) {
                ObtainTimeStamp();
                av_packet_unref(m_packet);
                return m_frame;
            } else {
                LOGI("Receive frame error result: %s", av_err2str(AVERROR(result)));
            }
        }
        //释放packet
        av_packet_unref(m_packet);
        ret = av_read_frame(m_format_ctx, m_packet);
    }

    av_packet_unref(m_packet);
    LOGI("ret = %d", ret);
    return nullptr;
}


void BaseDecoder::ObtainTimeStamp() {
    if (m_frame->pkt_dts != AV_NOPTS_VALUE) {
        m_cur_t_s = m_packet->dts;
    } else if (m_frame->pts != AV_NOPTS_VALUE) {
        m_cur_t_s = m_frame->pts;
    } else {
        m_cur_t_s = 0;
    }

    m_cur_t_s = (int64_t) ((m_cur_t_s * av_q2d(m_format_ctx->streams[m_stream_index]->time_base)) *
                           1000);
}

void BaseDecoder::SyncRender() {
    if (ForSynthesizer()) {
        return;
    }
    int64_t ct = GetCurMsTime();
    int64_t passTime = ct - m_started_t;
    if (m_cur_t_s > passTime) {
        av_usleep((unsigned int) ((m_cur_t_s - passTime) * 1000));
    }
}

void BaseDecoder::DoneDecode(JNIEnv *env) {
    LOGI("Decode done and decoder release");
    //释放缓存
    if (m_packet != nullptr) {
        av_packet_free(&m_packet);
    }
    if (m_frame != nullptr) {
        av_frame_free(&m_frame);
    }
    //关闭解码器
    if (m_codec_ctx != nullptr) {
        avcodec_close(m_codec_ctx);
        avcodec_free_context(&m_codec_ctx);
    }

    //关闭输入流
    if (m_format_ctx != nullptr) {
        avformat_close_input(&m_format_ctx);
        avformat_free_context(m_format_ctx);
    }

    //释放转换参数
    if (m_path_ref != nullptr && m_path != nullptr) {
        env->ReleaseStringUTFChars((jstring) m_path_ref, m_path);
        env->DeleteGlobalRef(m_path_ref);
    }

    //通知子类释放资源
    Release();
}

void BaseDecoder::Wait(long second, long ms) {
    pthread_mutex_lock(&m_mutex);
    if (second > 0 || ms > 0) {
        timeval now{};
        timespec outtime{};
        gettimeofday(&now, nullptr);
        int64_t destNSec = now.tv_usec * 1000 + ms * 1000000;
        outtime.tv_sec = static_cast<__kernel_time_t>(now.tv_sec + second + destNSec / 1000000000);
        outtime.tv_nsec = static_cast<long>(destNSec % 1000000000);
        pthread_cond_timedwait(&m_cond, &m_mutex, &outtime);
    } else {
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    pthread_mutex_unlock(&m_mutex);
}

void BaseDecoder::SendSignal() {
    pthread_mutex_lock(&m_mutex);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

void BaseDecoder::GoOn() {
    m_state = DECODING;
    SendSignal();
}

void BaseDecoder::Pause() {
    m_state = PAUSE;
}

void BaseDecoder::Stop() {
    m_state = STOP;
    SendSignal();
}

bool BaseDecoder::IsRunning() {
    return DECODING == m_state;
}

long BaseDecoder::GetDuration() {
    return m_duration;
}

long BaseDecoder::GetCurPos() {
    return (long) m_cur_t_s;
}