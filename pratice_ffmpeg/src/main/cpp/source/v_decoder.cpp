//
// Created by shixin on 2022/10/12.
//

#include "v_decoder.h"


VideoDecoder::VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer)
        : BaseDecoder(env, path, for_synthesizer) {
}

VideoDecoder::~VideoDecoder() {
    delete m_video_render;
}

void VideoDecoder::setRender(VideoRender *render) {
    this->m_video_render = render;
}

void VideoDecoder::Prepare(JNIEnv *env) {
    InitRender(env);
    InitBuffer();
    InitSws();
}


void VideoDecoder::InitRender(JNIEnv *env) {
    if (m_video_render != NULL) {
        int dst_size[2] = {-1, -1};
        m_video_render->InitRender(env, width(), height(), dst_size);
        m_dst_w = dst_size[0];
        m_dst_h = dst_size[1];
        if (m_dst_w == -1) {
            m_dst_w = width();
        }

        if (m_dst_h == -1) {
            m_dst_h = height();
        }

        LOGI("获取视频的宽高  dst %d, %d", m_dst_w, m_dst_h);
    } else {
        LOGE("Init render error, you should call SetRender first!");
    }
}

void VideoDecoder::InitBuffer() {
    m_rgb_frame = av_frame_alloc();
    //获取缓存大小
    int numBytes = av_image_get_buffer_size(DST_FORMAT, m_dst_w, m_dst_h, 1);
    //分配内存
    m_buf_for_rgb_frame = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));


    //讲内存分配给RGBframe,并将内存格式化为三个通道后，分别保存其地址
    av_image_fill_arrays(m_rgb_frame->data, m_rgb_frame->linesize, m_buf_for_rgb_frame, DST_FORMAT,
                         m_dst_w, m_dst_w, 1);

}

void VideoDecoder::InitSws() {
    //初始化格式转换工具
    //在sws_getContext()中更改想要转换的格式：
    //                int srcW, /* 输入图像的宽度 */
    //                int srcH, /* 输入图像的宽度 */
    //                enum AVPixelFormat srcFormat, /* 输入图像的像素格式 */
    //                int dstW, /* 输出图像的宽度 */
    //                int dstH, /* 输出图像的高度 */
    //                enum AVPixelFormat dstFormat, /* 输出图像的像素格式 */
    //                int flags,/* 选择缩放算法(只有当输入输出图像大小不同时有效),一般选择SWS_FAST_BILINEAR */
    //                SwsFilter *srcFilter, /* 输入图像的滤波器信息, 若不需要传NULL */
    //                SwsFilter *dstFilter, /* 输出图像的滤波器信息, 若不需要传NULL */
    //                const double *param /* 特定缩放算法需要的参数(?)，默认为NULL */

    m_sws_ctx = sws_getContext(width(),
                               height(),
                               video_pixel_format(),
                               m_dst_w,
                               m_dst_h,
                               DST_FORMAT,
                               SWS_FAST_BILINEAR,
                               nullptr,
                               nullptr,
                               nullptr);
}

void VideoDecoder::Render(AVFrame *frame) {

    //int sws_scale(struct SwsContext *c, const uint8_t *const srcSlice[],
    //              const int srcStride[], int srcSliceY, int srcSliceH,
    //              uint8_t *const dst[], const int dstStride[]);

    //1.参数 SwsContext *c， 转换格式的上下文。也就是 sws_getContext 函数返回的结果。
    //2.参数 const uint8_t *const srcSlice[], 输入图像的每个颜色通道的数据指针。其实就是解码后的AVFrame中的data[]数组。因为不同像素的存储格式不同，所以srcSlice[]维数
    //也有可能不同。
    //以YUV420P为例，它是planar格式，它的内存中的排布如下：
    //YYYYYYYY UUUU VVVV
    //使用FFmpeg解码后存储在AVFrame的data[]数组中时：
    //data[0]——-Y分量, Y1, Y2, Y3, Y4, Y5, Y6, Y7, Y8……
    //data[1]——-U分量, U1, U2, U3, U4……
    //data[2]——-V分量, V1, V2, V3, V4……
    //linesize[]数组中保存的是对应通道的数据宽度 ，
    //linesize[0]——-Y分量的宽度
    //linesize[1]——-U分量的宽度
    //linesize[2]——-V分量的宽度
    //
    //而RGB24，它是packed格式，它在data[]数组中则只有一维，它在存储方式如下：
    //data[0]: R1, G1, B1, R2, G2, B2, R3, G3, B3, R4, G4, B4……
    //这里要特别注意，linesize[0]的值并不一定等于图片的宽度，有时候为了对齐各解码器的CPU，实际尺寸会大于图片的宽度，
    // 这点在我们编程时（比如OpengGL硬件转换/渲染）要特别注意，否则解码出来的图像会异常。
    //
    //3.参数const int srcStride[]，输入图像的每个颜色通道的跨度。.也就是每个通道的行字节数，
    // 对应的是解码后的AVFrame中的linesize[]数组。根据它可以确立下一行的起始位置，不过stride和width不一定相同，这是因为：
    //a.由于数据帧存储的对齐，有可能会向每行后面增加一些填充字节这样 stride = width + N；
    //b.packet色彩空间下，每个像素几个通道数据混合在一起，例如RGB24，每个像素3字节连续存放，因此下一行的位置需要跳过3*width字节。
    //
    //4.参数int srcSliceY, int srcSliceH,定义在输入图像上处理区域，srcSliceY是起始位置，srcSliceH是处理多少行。
    // 如果srcSliceY=0，srcSliceH=height，表示一次性处理完整个图像。这种设置是为了多线程并行，例如可以创建两个线程，
    // 第一个线程处理 [0, h/2-1]行，第二个线程处理 [h/2, h-1]行。并行处理加快速度。
    //5.参数uint8_t *const dst[], const int dstStride[]定义输出图像信息（输出的每个颜色通道数据指针，每个颜色通道行字节数）
    sws_scale(m_sws_ctx, frame->data,
              frame->linesize,
              0,
              height(),
              m_rgb_frame->data,
              m_rgb_frame->linesize);
    auto *oneFrame = new OneFrame(m_rgb_frame->data[0],
                                  m_rgb_frame->linesize[0], frame->pts,
                                  time_base(), NULL,
                                  false);
    m_video_render->Render(oneFrame);
}

bool VideoDecoder::NeedLoopDecode() {
    return true;
}


void VideoDecoder::Release() {
    LOGE(TAG, "[VIDEO] release")
    if (m_rgb_frame != NULL) {
        av_frame_free(&m_rgb_frame);
        m_rgb_frame = NULL;
    }
    if (m_buf_for_rgb_frame != NULL) {
        free(m_buf_for_rgb_frame);
        m_buf_for_rgb_frame = NULL;
    }
    if (m_sws_ctx != NULL) {
        sws_freeContext(m_sws_ctx);
        m_sws_ctx = NULL;
    }
    if (m_video_render != NULL) {
        m_video_render->ReleaseRender();
        m_video_render = NULL;
    }
}
