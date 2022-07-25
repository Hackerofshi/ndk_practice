//
// Created by shixin on 2022/7/19.
//
#ifdef ANDROID

#include <android/log.h>

#define LOG_TAG    "TEST"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

#include <string.h>
#include <malloc.h>

void testStr1() {
    char buff[100] = {'a', 'b', 'c', 'd'}; //5-99 都是默认值 0

    // len =5
    int len = strlen(buff); // 碰到\0 就结束了  size 4

    //size 100
    //int size = sizeof(buff);

    //内容  buff[120]没有越界
    //LOGI("%d,%d,%d,%d", buff[0], buff[66], buff[77], buff[120]);

    char buff1[100] = {0}; //数组初始化为0

    char buff2[100] = {}; //数据都是默认值

    char buff3[] = "123456";  //len 6  size是7 末尾是\0

    char *buff4 = "123456";


}


// 设计一个获取 Bitmap 属性的函数，请问怎么设计？（重点）
// 1. 确定你的参数 传递指针
// 2. 一定要考虑健壮性
// 3. 要异常错误进行抛出说明 音视频 ffmpeg
// 4. 不要直接轻易的去改变调用传递给你的指针 打个比方

typedef struct AndroidBitmapInfo {
    int width;
    int height;
    int format;
} AndroidBitmapInfo;// 定义了一个结构体取了一个别名 AndroidBitmapInfo ，结构体的名字是 匿名

int AndroidBitmap_getInfo(struct AndroidBitmapInfo *bitmapInfo) {

    // 对 bitmapInfo 进行再次赋值

    //  2. 一定要考虑健壮性 , 自己的方法会报错
    if (bitmapInfo == NULL) {
        // 3. 要异常错误进行抛出说明 音视频 ffmpeg
        printf("bitmapInfo 是 NULL 无法进行操作，错误码 %d", -1);
        return -1;
    }
    // 可能还会有 -xx

    // 通过指针在方法里面赋值
    bitmapInfo->width = 200;
    bitmapInfo->height = 200;
    return 0;// 一般 0 代表正常情况
}

// 模拟 AndroidBitmap_getInfo
int main() {
    // 通过指针间接赋值很常见 (堆内存上开闭)
    struct AndroidBitmapInfo *androidBitmapInfo = (AndroidBitmapInfo *) malloc(
            sizeof(AndroidBitmapInfo));

    // 释放后再调用
    int res = AndroidBitmap_getInfo(androidBitmapInfo);

    if (res == 0) {
        // ...
        // 会出错 ， 调用者也会报错，但我并不知道错误所在
        printf("width = %d, height = %d", androidBitmapInfo->width, androidBitmapInfo->height);
    }

    // 释放
    if (androidBitmapInfo != NULL) {
        free(androidBitmapInfo);
        // 避免出现野指针的情况，减少程序代码的出错
        androidBitmapInfo = NULL;
    }

    getchar();
}

// 4. 不要直接轻易的去改变调用传递给你的指针 打个比方
// 指针改变，原来的值也会改变
int strlen1(char *const str) {
    // 临时变量,套路，不要操作传入的变量。
    char *countStr = str;

    int count = 0;
    while (*countStr)// != '\0'
    {
        countStr++;
        count++;
    }

    // 自己想看看结果对不对
    printf("str = %s,len = %d\n", str, count);

    return count;
}

void main_str2() {
    char *str = "123456";

    int len = strlen1(str);

    printf("len = %d", len);
    getchar();
}



