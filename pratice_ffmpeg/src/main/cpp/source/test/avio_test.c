

//
// Created by 78303 on 2024/8/21.
//



#include "../../ffmpeg/include/libavcodec/avcodec.h"
#include "../../ffmpeg/include/libavformat/avio.h"
#include "../../ffmpeg/include/libavformat/avformat.h"
#include "../../ffmpeg/include/libavutil/log.h"
#include "../../ffmpeg/include/libavutil/frame.h"
#include "libavutil/file.h"
#include <stdio.h>
#include "avio_test.h"
#include "../../utils/log.h"

#define BUF_SIZE 20480

FILE *in_file = NULL;
struct buffer_data {
    uint8_t *ptr;
    uint8_t *ori_ptr;  // for seek file stream
    size_t size;       ///< size left in the buffer
    size_t file_size;  ///< size of the file to decode
};

static char *av_get_err(int errnum) {
    static char err_buf[128] = {0};
    av_strerror(errnum, err_buf, 128);
    return err_buf;
}

static void print_sample_format(const AVFrame *frame) {
    LOGI("ar-samplerate: %uHz\n", frame->sample_rate)
    LOGI("ac-channel: %u\n", frame->channels)
    LOGI("f-format: %u\n",
         frame->format)  // 格式需要注意，实际存储到本地文件时已经改成交错模式
}

/*
int read_size;
static int read_packet(void *opaque, uint8_t *buf, int buf_size) {
  //    FILE *in_file = (FILE *)opaque;
  read_size = fread(buf, 1, buf_size, in_file);
       LOGI( "read_packet read_size:%d, buf_size:%d\n", read_size, buf_size);
  if (read_size <= 0) {
    return AVERROR_EOF;  // 数据读取完毕
  }
  return read_size;
}
*/
static int read_packet(void *opaque, uint8_t *buf, int buf_size) {
    struct buffer_data *bd = (struct buffer_data *) opaque;
    buf_size = FFMIN(buf_size, bd->size);

    if (!buf_size) return AVERROR_EOF;

    /* copy internal buffer data to buf */
    memcpy(buf, bd->ptr, buf_size);
    bd->ptr += buf_size;
    bd->size -= buf_size;
    LOGI("=========> ptr:%p size:%zu buf_size: %d\n", bd->ptr, bd->size, buf_size)

    return buf_size;
}

// for some format like ALAC (apple format) , which moov partten is located at
// the end of file so we need to implement seek function during demux to seek to
// the end of file for paring the moov info and then seek back to the front
static int64_t seek_packet(void *opaque, int64_t offset, int whence) {
    //    FILE *in_file = (FILE *)opaque;
    struct buffer_data *bd = (struct buffer_data *) opaque;
    int64_t ret = -1;
    LOGI("whence=%d , offset=%lld \n", whence, offset)
    switch (whence) {
        case AVSEEK_SIZE:
            LOGI("AVSEEK_SIZE \n")
            ret = bd->file_size;
            break;
        case SEEK_SET:
            LOGI("SEEK_SET \n")
            bd->ptr = bd->ori_ptr + offset;
            bd->size = bd->file_size - offset;
            ret = (int64_t) bd->ptr;
            break;

        case SEEK_CUR:
            LOGI("SEEK_cur \n")
            break;
        case SEEK_END:
            LOGI("SEEK_end \n")
            break;
        default:
            LOGI("default \n")
            break;
    }
    return ret;
}

static void decode(AVCodecContext *dec_ctx, AVPacket *packet, AVFrame *frame,
                   FILE *outfile) {
    int ret = 0;
    ret = avcodec_send_packet(dec_ctx, packet);
    if (ret == AVERROR(EAGAIN)) {
        LOGI("Receive_frame and send_packet both returned EAGAIN, which is an API "
             "violation.\n")
    } else if (ret < 0) {
        LOGI("Error submitting the packet to the decoder, err:%s\n",
             av_get_err(ret))
        return;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            return;
        } else if (ret < 0) {
            LOGI("Error during decoding\n")
            exit(1);
        }
        if (!packet) {
            LOGI("get flush frame\n")
        }
        int out_sample_bytes = av_get_bytes_per_sample(dec_ctx->sample_fmt);
        int out_sample_is_plannar = av_sample_fmt_is_planar(dec_ctx->sample_fmt);
        //       LOGI( "debug %d is out_sample_is_plannar : %d \n", __LINE__,
        //       out_sample_is_plannar);
        // print_sample_format(frame);
        if (out_sample_bytes < 0) {
            /* This should not occur, checking just for paranoia */
            fprintf(stderr, "Failed to calculate data size\n");
            exit(1);
        }

        // LOGI( "debug %d out_sample_bytes: %d samples: %d ch:%d\n", __LINE__,
        //  out_sample_bytes, frame->nb_samples,
        //  dec_ctx->ch_layout.nb_channels);
        if (out_sample_is_plannar) {  // plannar frames
            LOGI("out_sample_is_plannar\n")
            /**
               P表示Planar（平面），其数据格式排列方式为 :
               LLLLLLRRRRRRLLLLLLRRRRRRLLLLLLRRRRRRL...（每个LLLLLLRRRRRR为一个音频帧）
               而不带P的数据格式（即交错排列）排列方式为：
               LRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRL...（每个LR为一个音频样本）
            播放范例：   ffplay -ar 48000 -ac 2 -f f32le believe.pcm
                并不是每一种都是这样的格式
            */
            // 这里的写法不是通用，通用要调用重采样的函数去实现
            // 这里只是针对解码出来是planar格式的转换

            for (int i = 0; i < frame->nb_samples; i++) {
                for (int ch = 0; ch < frame->channels; ch++) {
                    // for(int ch = 0; ch < 1; ch++) {
                    /*fwrite(frame->data[ch] + out_sample_bytes * i, 1, out_sample_bytes,
                           outfile);*/
                }
            }
        } else { // packed frame
            /*fwrite(frame->data[0],
                   frame->nb_samples * out_sample_bytes * frame->channels, 1,
                   outfile);*/
            uint8_t *res = frame->data[0];
            LOGI("packed frame %lld\n", frame->pts)
        }
    }
}


int main(int argc, char **argv) {
    if (argc != 3) {
        LOGI("usage: %s <intput file> <out file>\n", argv[0])
        return -1;
    }

    return 0;
}

int testPath(const char *str) {
    av_log_set_level(AV_LOG_TRACE);
    const char *in_file_name = str;
    //const char *out_file_name = argv[2];
    //    FILE *in_file = NULL;
    //FILE *out_file = NULL;

    // 1. 打开参数文件
    in_file = fopen(in_file_name, "rb");
    if (!in_file) {
        LOGI("open file %s failed\n", in_file_name)
        return -1;
    }
    /*out_file = fopen(out_file_name, "wb+");
    if (!out_file) {
              LOGI( "open file %s failed\n", out_file_name);
        return -1;
    }*/

    struct buffer_data bd = {0};
    uint8_t *buffer = NULL;
    size_t buffer_size;
    int ret = av_file_map(in_file_name, &buffer, &buffer_size, 0, NULL);
    LOGI("file size: %d\n", buffer_size)
    LOGI("ret: %d\n", ret)
    bd.ptr = buffer;
    bd.ori_ptr = buffer;
    bd.file_size = buffer_size;
    bd.size = buffer_size;

    //    AVInputFormat* in_fmt = av_find_input_format("flac");
    // 2自定义 io
    uint8_t *io_buffer = av_malloc(BUF_SIZE);
    // AVIOContext *avio_ctx = avio_alloc_context(io_buffer, BUF_SIZE, 0, (void*)in_file,
    AVIOContext *avio_ctx = avio_alloc_context(io_buffer, BUF_SIZE, 0, &bd,
                                               read_packet, NULL, seek_packet);
    // avio_alloc_context(io_buffer, BUF_SIZE, 0, &bd, read_packet, NULL, NULL);
    AVFormatContext *format_ctx = avformat_alloc_context();
    format_ctx->pb = avio_ctx;
    format_ctx->flags = AVFMT_FLAG_CUSTOM_IO;
    // int ret = avformat_open_input(&format_ctx, NULL, in_fmt, NULL);
    // 从输入源读取封装格式文件头
    ret = avformat_open_input(&format_ctx, NULL, NULL, NULL);
    if (ret < 0) {
        LOGI("avformat_open_input failed:%s\n", av_err2str(ret))
        return -1;
    }

    // 从输入源读取一段数据，尝试解码，以获取流信息
    if ((ret = avformat_find_stream_info(format_ctx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    av_dump_format(format_ctx, 0, NULL, 0);

    int videoStreamIndex = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO,
                                               -1, -1, NULL, 0);
    LOGI("==============>%d", (videoStreamIndex))
    AVStream *st = format_ctx->streams[videoStreamIndex];
    // 编码器查找
    // AVCodec *codec = avcodec_find_decoder(AV_CODEC_ID_ALAC);
    // set codec id from famat params
    AVCodec *codec = avcodec_find_decoder(st->codecpar->codec_id);
    if (!codec) {
        LOGI("avcodec_find_decoder failed\n")
        return -1;
    }

    AVCodecContext *codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        LOGI("avcodec_alloc_context3 failed\n")
        return -1;
    }

    // copy params from format to codec
    ret = avcodec_parameters_to_context(
            codec_ctx, format_ctx->streams[videoStreamIndex]->codecpar);
    if (ret < 0) {
        LOGI("Failed to copy in_stream codecpar to codec context\n")
    }
    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret < 0) {
        LOGI("avcodec_open2 failed:%s\n", av_err2str(ret))
        return -1;
    }

    LOGI("debug codec_ctx->sample_rate: %d\n", codec_ctx->sample_rate)
    AVPacket *packet = av_packet_alloc();
    AVFrame *frame = av_frame_alloc();

    while (1) {
        ret = av_read_frame(format_ctx, packet);
        if (ret < 0) {
            LOGI("av_read_frame failed:%s\n", av_err2str(ret))
            break;
        }

        decode(codec_ctx, packet, frame, NULL);
    }

    LOGI("read file finish\n")
    decode(codec_ctx, NULL, frame, NULL);

    fclose(in_file);
    //fclose(out_file);

    av_frame_free(&frame);
    av_packet_free(&packet);

    avformat_close_input(&format_ctx);
    avcodec_free_context(&codec_ctx);

    LOGI("main finish\n")
    return 0;
}
