//
// Created by shixin on 2022/12/20.
//
#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include <android/log.h>
#include "cv_helper.h"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

#define TAG "JNI_TAG"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)


// 使用命名空间
using namespace cv;
using namespace std;

extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_findQrCodeSimple(JNIEnv *env, jclass clazz,
                                                             jobject bitmap) {

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    //resize(src, src, Size(), 0.5, 0.5);

    Mat gary;
    cvtColor(src, gary, COLOR_BGRA2GRAY);

    //模糊
    Mat imgBlur;
    GaussianBlur(gary, imgBlur, Size(15.0, 15.0), 0);


    // 梯度和二值化
    Mat imgCanny;
    Canny(imgBlur, imgCanny, 50, 150);

    // 膨胀
    Mat imgDil;

    //卷积核
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(imgCanny, imgDil, kernel);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    //查找边缘
    findContours(imgDil, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());


    vector<vector<Point>> points;
    for (int i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area < 49) {
            continue;
        }
        int k = i;
        int c = 0;

        //drawContours(src, contours, i, Scalar(255,255), 1, 8, hierarchy);

        while (hierarchy[k][2] != -1.0) {
            k = hierarchy[k][2];
            c = c + 1;
            // LOGE("c======>%d", k);
            if (c >= 5) {
                points.push_back(contours[i]);
                //Scalar(255,0,0,255) 要用a r g b，不然没法显示颜色
                drawContours(src, contours, i, Scalar(255,0,0,255), 1, 8, hierarchy);
            }
        }
    }
    LOGE("个数======>%d", points.size());
    vector<Point> pts;
    for (auto & point : points) {
        pts.insert(pts.end(),point.begin(),point.end());
        LOGE("points.size()======>%d", point.size());
    }
    LOGE("pts.size()======>%d", pts.size());

    //Point2f
    RotatedRect rotatedRect = minAreaRect(pts);
    Rect qrRect = rotatedRect.boundingRect();

    Point2f points1[4];
    rotatedRect.points(points1);

    for (int j = 0; j < 4; j++)
    {
        line(src, points1[j], points1[(j + 1) % 4], Scalar(255,0, 0, 255), 2);
    }

    Mat qrCodeImg = Mat(src, qrRect);
    LOGE("qrCodeImg.cols()======>%d", qrCodeImg.cols);
    LOGE("qrCodeImg.rows()======>%d", qrCodeImg.rows);




    jobject bitmap1 = cv_helper::createBitMap(env, src.cols, src.rows, 0);
    cv_helper::mat2bitmap(env, src, bitmap1);
    return bitmap1;
}