//
// Created by shixin on 2022/10/12.
//
#include "timer.h"


int64_t GetCurMsTime1() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t ts = (int64_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ts;
}
