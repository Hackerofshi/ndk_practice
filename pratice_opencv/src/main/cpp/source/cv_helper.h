//
// Created by shixin on 2022/10/9.
//

#ifndef NDK_PRACTICE_CV_HELPER_H
#define NDK_PRACTICE_CV_HELPER_H

#include <jni.h>
#include "opencv2/opencv.hpp"
#include <android/bitmap.h>

class cv_helper {
public:
    static int bitmap2mat(JNIEnv *env, jobject &bitmap, cv::Mat &dst);
    static int mat2bitmap(JNIEnv *env,cv::Mat &src,jobject &bitmap);
    static jobject createBitMap(JNIEnv *env,jint width,jint height,int type);

};


#endif //NDK_PRACTICE_CV_HELPER_H
