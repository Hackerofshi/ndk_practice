//
// Created by shixin on 2022/10/12.
//

#ifndef NDK_PRACTICE_V_DECODER_H
#define NDK_PRACTICE_V_DECODER_H

#include "base_decoder.h"
#include <jni.h>
#include "../render/video_render.h"
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include "../render/one_frame.h"
#include "test/base_decoder1.h"
#include "../utils/timer.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
};


class VideoDecoder : public BaseDecoder {
private:
    const char *TAG = "VideoDecoder";
    //视频数据目标格式  RGBA
    const AVPixelFormat DST_FORMAT = AV_PIX_FMT_RGBA;
    //存放YUV转RGB后的数据
    AVFrame *m_rgb_frame = NULL;

    uint8_t *m_buf_for_rgb_frame = NULL;

    //视频格式转换器
    SwsContext *m_sws_ctx = NULL;

    //视频渲染器
    VideoRender *m_video_render = NULL;

    //显示的目标宽
    int m_dst_w;

    //显示的目标高
    int m_dst_h;

    /**
     * 初始化渲染器
     */
    void InitRender(JNIEnv *env);

    /**
     * 初始化显示器
     * @param env
     */
    void InitBuffer();

    /**
     * 初始化视频数据转换器
     */
    void InitSws();

public:
    VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer = false);

    ~VideoDecoder();

    void setRender(VideoRender *render);

protected:
    AVMediaType GetMediaType() override {
        return AVMEDIA_TYPE_VIDEO;
    }

    /**
     * 是否需要循环解码
     */
    bool NeedLoopDecode() override;

    /**
      * 准备解码环境
      * 注：在解码线程中回调
      * @param env 解码线程绑定的jni环境
      */
    void Prepare(JNIEnv *env) override;

    /**
     * 渲染
     * 注：在解码线程中回调
     * @param frame 解码RGBA数据
     */
    void Render(AVFrame *frame) override;

    /**
     * 释放回调
     */
    void Release() override;

    const char *const LogSpec() override {
        return "VIDEO";
    };
};


#endif //NDK_PRACTICE_V_DECODER_H
