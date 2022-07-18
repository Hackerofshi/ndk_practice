//
// Created by shixin on 2022/7/18.
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

struct MyStruct {
    char *name;
};

// int32_t ，可以对基本数据类型取别名
typedef int int32;

typedef long long long_64;

typedef struct MyStruct Student;// 给 MyStruct 取一个别名叫做 Student


void testStruct() {
    Student stu = { "Darren" };

    int32 number1 = 100;

    LOGI("number1 = %d",number1);

    // 获取 Bitmap ，锁定画布 void**
    // void number;// 错误

    // memcpy();// 拷贝内存，什么用 void*

}