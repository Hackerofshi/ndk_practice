//
// Created by shixin on 2022/8/5.
//

#include <jni.h>
#include <string>
#include <malloc.h>
#include <android/bitmap.h>
#include "../util/LogUtil.h"

#include <iostream>
#include <exception>

using namespace std;


void write_JPEG_file(uint8_t *data, int w, int h, jint q, const char *path) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Compressor_native_1Compress(
        JNIEnv *env,
        jclass clazz,
        jobject bitmap, jint q,
        jstring path_) {
    const char *path = env->GetStringUTFChars(path_, 0);
    //从bitmap获取rgba数据
    AndroidBitmapInfo info;//info=new 对象();
    //获取里面的信息
    AndroidBitmap_getInfo(env, bitmap, &info);//  void method(list)
    //得到图片中的像素信息
    uint8_t *pixels;//uint8_t char    java   byte     *pixels可以当byte[]


    AndroidBitmap_lockPixels(env, bitmap, (void **) &pixels);


    //jpeg argb中去掉他的a ===>rgb
    int w = info.width;
    int h = info.height;
    int color;
    ////data中可以存放图片的所有内容
    uint8_t *data = (uint8_t *) malloc(w * h * 3);
    uint8_t *temp = data;
    uint8_t r, g, b;//byte
    //循环取图片的每一个像素
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            color = *(int *) pixels;//0-3字节  color4 个字节  一个点
            //取出rgb
            r = (color >> 16) & 0xFF;//    #00rrggbb  16  0000rr   8  00rrgg
            g = (color >> 8) & 0xFF;
            b = color & 0xFF;
            //存放，以前的主流格式jpeg    bgr
            *data = b;
            *(data + 1) = g;
            *(data + 2) = r;
            data += 3;
            //指针跳过4个字节
            pixels += 4;
        }
    }
    //把得到的新的图片的信息存入一个新文件 中
    write_JPEG_file(temp, w, h, q, path);

    //释放内存
    free(temp);
    AndroidBitmap_unlockPixels(env, bitmap);
    env->ReleaseStringUTFChars(path_, path);
}



extern "C"
JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Compressor_convertBmp(JNIEnv *env, jobject thiz,
                                                              jobject jsrcBitmap,
                                                              jobject desBitmap) {
    AndroidBitmapInfo srcInfo, dstInfo;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, jsrcBitmap, &srcInfo)
        || ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, desBitmap, &dstInfo)) {
            LOGCATE("get bitmap info failed");
            return;
    }
    LOGCATE("get bitmap info success");

    uint8_t *srcBuf, *dstBuf;
    if (ANDROID_BITMAP_RESULT_SUCCESS !=
        AndroidBitmap_lockPixels(env, jsrcBitmap, (void **) &srcBuf)) {
        LOGCATE("lock src bitmap failed");
        return;
    }
    LOGCATE("lock bitmap info success");
    if (ANDROID_BITMAP_RESULT_SUCCESS !=
        AndroidBitmap_lockPixels(env, desBitmap, (void **) &dstBuf)) {
        LOGCATE("lock dst bitmap failed");
        return;
    }

    LOGCATE("width=%d; height=%d; stride=%d; format=%d;flag=%d",
            srcInfo.width, //  width=2700 (900*3)
            srcInfo.height, // height=2025 (675*3)
            srcInfo.stride, // stride=10800 (2700*4)
            srcInfo.format, // format=1 (ANDROID_BITMAP_FORMAT_RGBA_8888=1)
            srcInfo.flags); // flags=0 (ANDROID_BITMAP_RESULT_SUCCESS=0)

    int w = srcInfo.width;
    int h = srcInfo.height;
    int32_t *srcPixs = (int32_t *) srcBuf;
    int32_t *desPixs = (int32_t *) dstBuf;
    int alpha = 0xFF << 24;
    int i, j;
    int color;
    int red;
    int green;
    int blue;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            // get the color of per pixel
            color = srcPixs[w * i + j];
            red = ((color & 0x00FF0000) >> 16);
            green = ((color & 0x0000FF00) >> 8);
            blue = color & 0x000000FF;
            color = (red + green + blue) / 3;
            color = alpha | (color << 16) | (color << 8) | color;
            desPixs[w * i + j] = color;
        }
    }
    AndroidBitmap_unlockPixels(env, jsrcBitmap);
    AndroidBitmap_unlockPixels(env, desBitmap);
}