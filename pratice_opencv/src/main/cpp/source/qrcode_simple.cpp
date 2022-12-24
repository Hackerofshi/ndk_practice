//
// Created by shixin on 2022/11/24.
//
#include <jni.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat warpTransfrom(const Mat &gary, const RotatedRect &rect) {
    int width = rect.size.width;
    int height = rect.size.height;
    Mat result(Size(width, height), gary.type());

    // 矩阵怎么获取
    vector<Point> srcPoints;
    Point2f pts[4];
    rect.points(pts);
    for (int i = 0; i < 4; i++) {
        srcPoints.push_back(pts[i]);
    }
    vector<Point> dstPoints;
    dstPoints.push_back(Point(0, 0));
    dstPoints.push_back(Point(width, 0));
    dstPoints.push_back(Point(width, height));
    dstPoints.push_back(Point(0, height));

    Mat M = findHomography(srcPoints, dstPoints);
    warpPerspective(gary, result, M, result.size());

    return result;
}

bool isXCorner(const Mat &qrROI) {
    // 四个值
    int cb = 0, lw = 0, rw = 0, lb = 0, rb = 0;

    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;
    int cy = height / 2;
    uchar pixels = qrROI.at<uchar>(cy, cx);
    if (pixels == 255) {
        return false;
    }

    // 求中心黑色
    int start = 0, end = 0, offset = 0;
    bool findleft = false, findright = false;
    while (true) {
        offset++;

        if ((cx - offset) <= 0 || (cx + offset) >= width - 1) {
            break;
        }

        // 左边扫
        pixels = qrROI.at<uchar>(cy, cx - offset);
        if (!findleft && pixels == 255) {
            start = cx - offset;
            findleft = true;
        }

        // 右边扫
        pixels = qrROI.at<uchar>(cy, cx + offset);
        if (!findright && pixels == 255) {
            end = cx + offset;
            findright = true;
        }

        if (findleft && findright) {
            break;
        }
    }
    if (start == 0 || end == 0) {
        return false;
    }
    cb = end - start;
    // 相间的白色
    for (int col = end; col < width - 1; col++) {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 0) {
            break;
        }
        rw++;
    }
    for (int col = start; col > 0; col--) {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 0) {
            break;
        }
        lw++;
    }
    if (rw == 0 || lw == 0) {
        return false;
    }

    // 两边的黑色
    for (int col = end + rw; col < width - 1; col++) {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 255) {
            break;
        }
        rb++;
    }
    for (int col = start - lw; col > 0; col--) {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 255) {
            break;
        }
        lb++;
    }
    if (rb == 0 || lb == 0) {
        return false;
    }

    float sum = cb + lb + rb + lw + rw;
    // 求比例 3:1:1:1:1
    cb = (cb / sum) * 7.0 + 0.5;
    lb = (lb / sum) * 7.0 + 0.5;
    rb = (rb / sum) * 7.0 + 0.5;
    lw = (lw / sum) * 7.0 + 0.5;
    rw = (rw / sum) * 7.0 + 0.5;

    if ((cb == 3 || cb == 4) && (lw == rw) && (lb == rb) && (lw == rb) && (lw == 1)) {
        // 3:1:1:1:1 or 4:1:1:1:1
        return true;
    }

    return false;
}

// 最好还是加上
bool isYCorner(const Mat &qrROI) {
    // 统计白色像素点和黑色像素点
    int bp = 0, wp = 0;
    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;

    // 中心点是黑的
    int pv = 0;

    for (int row = 0; row < height; row++) {
        pv = qrROI.at<uchar>(row, cx);
        if (pv == 0) {
            bp++;
        } else if (pv == 255) {
            wp++;
        }
    }

    if (bp == 0 || wp == 0) {
        return false;
    }

    if (wp * 2 > bp || bp > 4 * wp) {
        return false;
    }

    return true;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_findQrCode(JNIEnv *env, jclass clazz,
                                                          jobject bitmap) {
    /*Mat src = imread("C:/Users/hcDarren/Desktop/android/code1.png");

    if (!src.data){
        printf("imread error!");
        return nullptr;
    }

    imshow("src", src);

    // 对图像进行灰度图
    Mat gary;
    cvtColor(src, gary, COLOR_BGR2GRAY);
    // 二值化
    threshold(gary, gary, 0, 255, THRESH_BINARY | THRESH_OTSU);
    imshow("threshold", gary);
    // 轮廓发现
    vector<vector<Point> > contours;
    findContours(gary, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

    // Mat result(gary.size(), CV_8UC3);
    for (int i = 0; i < contours.size(); i++)
    {
        // 过滤面积，
        double area = contourArea(contours[i]);
        if (area < 49){
            continue;
        }

        // 过滤宽高比和宽高大小
        RotatedRect rRect = minAreaRect(contours[i]);
        float w = rRect.size.width;
        float h = rRect.size.height;
        float ratio = min(w, h) / max(w, h);
        if (ratio > 0.9 && w< gary.cols/2 && h< gary.rows/2){
            // 去分析，找到满足宽高比的，满足宽高大小的
            Mat qrROI = warpTransfrom(gary, rRect);
            if (isYCorner(qrROI) && isXCorner(qrROI)) {
                drawContours(src, contours, i, Scalar(0, 0, 255), 4);
            }
        }
    }

    imshow("src", src);
    imwrite("C:/Users/hcDarren/Desktop/android/code_result.jpg", src);*/
    return nullptr;
}


//去除图片上的阴影




