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

void testStr1() {
    char buff[100] = {'a', 'b', 'c', 'd'}; //5-99 都是默认值 0

    // len =5
    //int len = strlen(buff); // 碰到\0 就结束了

    //size 100
    //int size = sizeof(buff);

    //内容  buff[120]没有越界
    //LOGI("%d,%d,%d,%d", buff[0], buff[66], buff[77], buff[120]);

}

