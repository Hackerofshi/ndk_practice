//
// Created by shixin on 2022/10/11.
//

#include <jni.h>
#include <string>
#include <unistd.h>
#include "media/player.h"
#include "player/gl_player.h"


extern "C" {
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <android/log.h>
#include "./source/test/avio_test.h"
#include "./source/test/pthread_test.h"
const char *TAG = "ff_native_lib";

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    av_jni_set_java_vm(vm, reserved);
    return JNI_VERSION_1_4;
}

JNIEXPORT jstring JNICALL
Java_com_tech_pratice_1ffmpeg_Player_ffmpegInfo(JNIEnv *env, jobject thiz) {
    char info[40000] = {0};
    AVCodec *c_temp = av_codec_next(nullptr);
    while (c_temp != nullptr) {
        if (c_temp->decode != nullptr) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }


    return env->NewStringUTF(info);
}

JNIEXPORT jint JNICALL
Java_com_tech_pratice_1ffmpeg_Player_createPlayer(JNIEnv *env, jobject thiz, jstring path,
                                                  jobject surface) {
    auto *player = new Player(env, path, surface);
    return (jint) player;
}
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_play(JNIEnv *env, jobject thiz, jint player) {
    auto *p = (Player *) player;
    p->play();
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_pause(JNIEnv *env, jobject thiz, jint player) {

}
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_stop(JNIEnv *env, jobject thiz, jint player) {
    auto *p = (Player *) player;
    p->Release();
}

JNIEXPORT jint JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_createGLPlayer(JNIEnv *env, jobject thiz,
                                                                    jstring path, jobject surface) {
    GLPlayer *player = new GLPlayer(env, path);
    player->SetSurface(surface);
    return (jint) player;
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_playOrPause(JNIEnv *env, jobject thiz,
                                                                 jint player) {
    GLPlayer *p = (GLPlayer *) player;
    p->PlayOrPause();
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_stop(JNIEnv *env, jobject thiz,
                                                          jint player) {
    auto *p = (GLPlayer *) player;
    p->Release();
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_testAVIOContext(JNIEnv *env, jobject thiz, jstring str) {
    const char *cStr = env->GetStringUTFChars(str, nullptr);
    testPath(cStr);
    env->ReleaseStringUTFChars(str, cStr);
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_startTCPClient(JNIEnv *env, jobject thiz, jstring ip,
                                                    jint port) {

    const char *server_ip = env->GetStringUTFChars(ip, nullptr);
    int server_port = port;

    int sockfd;
    struct sockaddr_in server_addr{};
    char buffer[1024];

    // 1. 创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("Socket creation error")
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    // 2. 将 IP 地址转换为二进制形式
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        LOGE("Invalid address/ Address not supported")
        close(sockfd);
        return;
    }

    // 3. 连接到服务器
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        LOGE("Connection Failed")
        close(sockfd);
        return;
    }

    LOGI("Connected to the server")

    // 4. 从服务器接收数据
    while (true) {
        ssize_t bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_read < 0) {
            LOGE("Read error")
            break;
        } else if (bytes_read == 0) {
            LOGI("Server closed the connection")
            break;
        }
        buffer[bytes_read] = '\0'; // 确保字符串以 nullptr 结尾
        LOGI("Received: %s", buffer)
    }
    LOG_INFO(TAG, "LogSpec()", "Fail to open file [%s]", "123")

    // 5. 关闭套接字
    close(sockfd);
    env->ReleaseStringUTFChars(ip, server_ip);
}

#define PORT 1234
#define BUFFER_SIZE 4096


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_startUDPClient(JNIEnv *env, jobject thiz) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr{};

    // 创建UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // 设置服务器地址
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定socket
    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        return;
    }

    while (true) {
        int n;
        socklen_t len = sizeof(servaddr);
        n = recvfrom(sockfd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL,
                     (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
        LOGI("Received: %d\n", n)
    }
    close(sockfd);
}
struct SocketParam {
    sockaddr_in servaddr;
    int sockfd;
};

// 自定义读取函数
int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    auto *socketParam = (struct SocketParam *) opaque;

    int n;
    socklen_t len = sizeof(socketParam->servaddr);
    n = recvfrom(socketParam->sockfd, buf, buf_size, MSG_WAITALL,
                 (struct sockaddr *) &(socketParam->servaddr), &len);
    if (n < 0) {
        LOGI("Failed to read from socket")
        return AVERROR(errno); // 返回一个 FFmpeg 错误码
    }
    //LOGI("读取数据 %d", n)
    return n;
}

// 自定义 seek 函数 (对于实时流, 通常不支持 seek, 返回 -1)
int64_t seek_packet(void *opaque, int64_t offset, int whence) {
    auto *socketParam = (struct SocketParam *) opaque;
    int64_t ret = -1;
    LOGI("whence=%d , offset=%lld \n", whence, offset)
    LOGI("sockfd=%d , ret=%lld \n", socketParam->sockfd, ret)
    return -1; // 实时流不支持 seek
}


void setup_avio_context(AVFormatContext *fmt_ctx, SocketParam *socketParam) {
    uint8_t *avio_ctx_buffer;
    size_t avio_ctx_buffer_size = 4096; // 缓冲区大小


    avio_ctx_buffer = (uint8_t *) av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        LOGE("Could not allocate buffer")
        return;
    }
    AVIOContext *avio_ctx = avio_alloc_context(
            avio_ctx_buffer, avio_ctx_buffer_size,
            0, // 写标志（0 表示只读）
            socketParam, // 传递自定义数据
            &read_packet, // 自定义读取函数
            nullptr, // 不需要写操作
            &seek_packet // 自定义 seek 函数
    );

    if (!avio_ctx) {
        LOGE("Could not allocate AVIOContext")
        av_free(avio_ctx_buffer);
        return;
    }

    fmt_ctx->pb = avio_ctx;
    //设置成自定义的IO
    fmt_ctx->flags = AVFMT_FLAG_CUSTOM_IO;

}

// 将 AV_PIX_FMT_YUV420P 类型的 AVFrame 数据保存为 YUV 文件
void save_yuv420p_to_file(const char *filename, AVFrame *frame) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        LOGE("Could not open file %s for writing", filename)
        return;
    }

    // 写入 Y 平面数据
    for (int i = 0; i < frame->height; i++) {
        fwrite(frame->data[0] + i * frame->linesize[0], 1, frame->width, file);
    }

    // 写入 U 平面数据
    for (int i = 0; i < frame->height / 2; i++) {
        fwrite(frame->data[1] + i * frame->linesize[1], 1, frame->width / 2, file);
    }

    // 写入 V 平面数据
    for (int i = 0; i < frame->height / 2; i++) {
        fwrite(frame->data[2] + i * frame->linesize[2], 1, frame->width / 2, file);
    }

    fclose(file);
    LOGI("YUV data saved to %s", filename)
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_testAVIOAndUDP(JNIEnv *env, jobject thiz, jstring jfilePath,
                                                    jobject surface) {
    const char *filePath = env->GetStringUTFChars(jfilePath, nullptr);
    LOGI("testAVIOAndUDP")
    avformat_network_init(); // 初始化网络模块（在 FFmpeg 3.x+ 版本中）

    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();
    AVFrame *pFrameYUV = av_frame_alloc();
    SwsContext *vctx = nullptr;
    ANativeWindow_Buffer wbuf;


    int sockfd;
    struct sockaddr_in servaddr{};

    // 创建UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        LOGI("socket creation failed")
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    LOGI("memset")
    // 设置服务器地址
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定socket
    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        LOGI("bind failed")
        close(sockfd);
        return;
    }
    LOGI("Connected to the server")

    // 创建 AVFormatContext
    AVFormatContext *fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        LOGI("Could not allocate format context\n")
        close(sockfd);
        return;
    }

    struct SocketParam param = {0};
    param.servaddr = servaddr;
    param.sockfd = sockfd;

    // 设置自定义 AVIOContext
    setup_avio_context(fmt_ctx, &param);

    // 打开输入（对于自定义输入，使用空字符串作为 URL）
    if (avformat_open_input(&fmt_ctx, nullptr, nullptr, nullptr) < 0) {
        LOGI("Could not open input\n")
        return;
    }

    // 读取流信息
    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        LOGI("Could not find stream info\n")
        return;
    }

    av_dump_format(fmt_ctx, 0, nullptr, 0);

    //4，查找编解码器
    //4.1 获取视频流的索引
    int videoStreamIndex = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_VIDEO,
                                               -1,
                                               -1, nullptr, 0);
    LOGI("==============>%d", (videoStreamIndex))


    //4.2 获取解码器参数
    AVStream *st = fmt_ctx->streams[videoStreamIndex];
    // 编码器查找
    // AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_ALAC);
    // set codec id from famat params

    //4.3 获取解码器
    AVCodec *codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec) {
        LOGI("avcodec_find_decoder failed\n")
        return;
    }


    //4.4 获取解码器上下文
    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        LOGI("avcodec_alloc_context3 failed\n")
        return;
    }

    // copy params from format to codec
    int ret = avcodec_parameters_to_context(
            codec_ctx, fmt_ctx->streams[videoStreamIndex]->codecpar);
    if (ret < 0) {
        LOGI("Failed to copy in_stream codecpar to codec context\n")
    }


    //5，打开解码器
    ret = avcodec_open2(codec_ctx, codec, nullptr);
    if (ret < 0) {
        LOGI("avcodec_open2 failed:%s\n", av_err2str(ret))
        return;
    }

    LOGI("debug codec_ctx->sample_rate: %d\n", codec_ctx->sample_rate)

    int outWidth = 1920;
    int outHeight = 1080;
    char *rgb = new char[outWidth * outHeight * 4];

    //显示窗口初始化
    ANativeWindow *nwin = ANativeWindow_fromSurface(env, surface);
    ANativeWindow_setBuffersGeometry(nwin, outWidth, outHeight,
                                     WINDOW_FORMAT_RGBA_8888);

    auto *out_buffer = new uint8_t[av_image_get_buffer_size(AV_PIX_FMT_YUV420P, codec_ctx->width,
                                                            codec_ctx->height, 1)];
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer,
                         AV_PIX_FMT_YUV420P,
                         codec_ctx->width,
                         codec_ctx->height, 1);

    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == videoStreamIndex) {
            ret = avcodec_send_packet(codec_ctx, packet);
            if (ret < 0) {
                LOGE("Error sending packet to decoder")
                break;
            }

            while (true) {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    LOGE("Error receiving frame from decoder")
                    break;
                }

                /*if (codec_ctx->pix_fmt == AV_PIX_FMT_YUV420P) {
                    LOGI("pix_fmt AV_PIX_FMT_YUV420P")
                } else if (codec_ctx->pix_fmt == AV_PIX_FMT_YUVJ420P) {
                    LOGI("pix_fmt AV_PIX_FMT_YUVJ420P")
                } else if (codec_ctx->pix_fmt == AV_PIX_FMT_NV12) {
                    LOGI("pix_fmt AV_PIX_FMT_NV12")
                } else if (codec_ctx->pix_fmt == AV_PIX_FMT_NV21) {
                    LOGI("pix_fmt AV_PIX_FMT_NV21")
                } else if (codec_ctx->pix_fmt == AV_PIX_FMT_RGBA) {
                    LOGI("pix_fmt AV_PIX_FMT_RGBA")
                } else {
                    LOGI("pix_fmt other")
                }*/



                /*if (frame->key_frame) {
                    LOGI("key_frame")
                } else {
                    LOGI("this is not key_frame")
                }

                LOGI("linesize 0 %d", frame->linesize[0])
                LOGI("linesize 1 %d", frame->linesize[1])
                LOGI("linesize 2 %d", frame->linesize[2])

                int len = sizeof(frame->data[0]);
                LOGI("data 0 %d", len)*/

                // 将YUV数据转换成RGB数据显示
                vctx = sws_getCachedContext(vctx,
                                            frame->width,
                                            frame->height,
                                            (AVPixelFormat) frame->format,
                                            outWidth,
                                            outHeight,
                                            AV_PIX_FMT_RGBA,
                                            SWS_FAST_BILINEAR,
                                            nullptr, nullptr, nullptr);

                if (!vctx) {
                    LOGE("sws_getCachedContext failed!")
                } else {
                    uint8_t *data[AV_NUM_DATA_POINTERS] = {nullptr};
                    data[0] = (uint8_t *) rgb;
                    int lines[AV_NUM_DATA_POINTERS] = {0};
                    lines[0] = outWidth * 4;
                    int h = sws_scale(vctx,
                                      (const uint8_t **) frame->data,
                                      frame->linesize, 0,
                                      frame->height,
                                      data, lines);
                    LOGE("sws_scale = %d", h)
                    if (h > 0) {
                        // 绘制
                        ANativeWindow_lock(nwin, &wbuf, nullptr);
                        auto *dst = (uint8_t *) wbuf.bits;
                        memcpy(dst, rgb, outWidth * outHeight * 4);
                        ANativeWindow_unlockAndPost(nwin);
                    }
                }


                //if (!flag) {
                /*SwsContext *img_convert_ctx = sws_getContext(codec_ctx->width,
                                                             codec_ctx->height,
                                                             codec_ctx->pix_fmt,
                                                             codec_ctx->width,
                                                             codec_ctx->height,
                                                             AV_PIX_FMT_RGB24, SWS_BICUBIC,
                                                             NULL, NULL, NULL);
                sws_scale(img_convert_ctx, (const uint8_t *const *) frame->data,
                          frame->linesize, 0, codec_ctx->height, pFrameYUV->data,
                          pFrameYUV->linesize);
                FILE *file = fopen(filePath, "wb");
                fwrite(pFrameYUV->data[0], (codec_ctx->width) * (codec_ctx->height) * 3, 1,
                       file);*/

                /*save_yuv420p_to_file(filePath, frame);*/

                /*SwsContext *img_convert_ctx = sws_getContext(codec_ctx->width,
                                                             codec_ctx->height,
                                                             codec_ctx->pix_fmt,
                                                             codec_ctx->width,
                                                             codec_ctx->height,
                                                             AV_PIX_FMT_YUV420P, SWS_BICUBIC,
                                                             NULL, NULL, NULL);
                sws_scale(img_convert_ctx, (const uint8_t *const *) frame->data,
                          frame->linesize, 0, codec_ctx->height, pFrameYUV->data,
                          pFrameYUV->linesize);


                //YUV420P
                FILE *output = fopen(filePath, "wb+");
                fwrite(pFrameYUV->data[0], (codec_ctx->width) * (codec_ctx->height), 1, output);
                fwrite(pFrameYUV->data[1], (codec_ctx->width) * (codec_ctx->height) / 4, 1,
                       output);
                fwrite(pFrameYUV->data[2], (codec_ctx->width) * (codec_ctx->height) / 4, 1,
                       output);
                flag = true;*/
            }

            // 处理解码后的帧
            //LOGI("Received frame %d", codec_ctx->frame_number);

        }
        av_packet_unref(packet);
    }

    if (frame != nullptr) {
        av_frame_free(&frame);
    }
    // 释放缓存
    if (packet != nullptr) {
        av_packet_free(&packet);
    }
    // 关闭解码器
    avcodec_close(codec_ctx);
    avcodec_free_context(&codec_ctx);

    // 关闭输入流
    if (fmt_ctx != nullptr) {
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
    }

    // 释放转换参数
    if (jfilePath != nullptr && filePath != nullptr) {
        env->ReleaseStringUTFChars(jfilePath, filePath);
        env->DeleteGlobalRef(jfilePath);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_testThread(JNIEnv *env, jobject thiz) {
    test_thread();
}

}




