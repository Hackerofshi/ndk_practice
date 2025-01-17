//
// Created by shixin on 2022/10/12.
//

#ifndef NDK_PRACTICE_LOG_H
#define NDK_PRACTICE_LOG_H

#include <android/log.h>

#define LOG_TAG  "FF_SHI_XIN"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__);
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__);

#define LOG_INFO(TAG, SPEC, FMT, ...) LOGI(TAG, "[%s] " FMT, SPEC, ##__VA_ARGS__)
#define LOG_ERROR(TAG, SPEC, FMT, ...) LOGE(TAG, "[%s] " FMT, SPEC, ##__VA_ARGS__)


#endif //NDK_PRACTICE_LOG_H
