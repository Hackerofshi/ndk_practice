//
// Created by shixin on 2022/8/29.
//

#include <jni.h>
#include <string>
#include "BitmapMatUtils.h"
#include <android/log.h>
#include "cardocr.h"
#include <vector>
#include <jni.h>

using namespace std;

extern "C"
JNIEXPORT jstring JNICALL Java_com_shixin_pratice_1opencv_BankCardOcr_cardOcr(JNIEnv *env, jclass type, jobject bitmap) {
    __android_log_print(ANDROID_LOG_ERROR, "TAG", "Java_com_darren_ocr_BankCardOcr_cardOcr");

    // 1. bitmap -> mat
    Mat mat;
    BitmapMatUtils::bitmap2mat(env, bitmap, mat);

    //  轮廓增强（梯度增强）
    // Rect card_area;
    // co1::find_card_area(mat,card_area);
    // 对我们过滤到的银行卡区域进行裁剪
    // Mat card_mat(mat,card_area);
    // imwrite("/storage/emulated/0/ocr/card_n.jpg",card_mat);

    // 截取到卡号区域
    Rect card_number_area;
    co1::find_card_number_area(mat,card_number_area);
    Mat card_number_mat(mat,card_number_area);
    imwrite("/storage/emulated/0/ocr/card_number_n.jpg",card_number_mat);


    // 获取数字
    vector<Mat> numbers;
    co1::find_card_numbers(card_number_mat,numbers);


    return env->NewStringUTF("622848");
}
