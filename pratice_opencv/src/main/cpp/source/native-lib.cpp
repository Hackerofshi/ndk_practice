//
// Created by shixin on 2022/8/29.
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
{
JNIEXPORT jint JNICALL
Java_com_shixin_pratice_1opencv_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject instance,
                                                                    jobject mFaceBitmap);
// bitmap 转成 Mat
void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap);
// mat 转成 Bitmap
void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap);

JNIEXPORT void JNICALL
Java_com_shixin_pratice_1opencv_FaceDetection_loadCascade(JNIEnv *env, jobject instance,
                                                          jstring filePath_);
}

void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap) {
    // Mat 里面有个 type ： CV_8UC4 刚好对上我们的 Bitmap 中 ARGB_8888 , CV_8UC2 刚好对象我们的 Bitmap 中 RGB_565
    // 1. 获取 bitmap 信息
    AndroidBitmapInfo info;
    void *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);

    // 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    // 指定 mat 的宽高和type  BGRA
    mat.create(info.height, info.width, CV_8UC4);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        // 对应的 mat 应该是  CV_8UC4
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        // 把数据 temp 复制到 mat 里面
        temp.copyTo(mat);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        // 对应的 mat 应该是  CV_8UC2
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        // mat 是 CV_8UC4 ，CV_8UC2 -> CV_8UC4
        cvtColor(temp, mat, COLOR_BGR5652BGRA);
    }
    // 其他要自己去转

    // 解锁 Bitmap 画布
    AndroidBitmap_unlockPixels(env, bitmap);
}

void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap) {
    // 1. 获取 bitmap 信息
    AndroidBitmapInfo info;
    void *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);

    // 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env, bitmap, &pixels);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {// C4
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        if (mat.type() == CV_8UC4) {
            mat.copyTo(temp);
        } else if (mat.type() == CV_8UC2) {
            cvtColor(mat, temp, COLOR_BGR5652BGRA);
        } else if (mat.type() == CV_8UC1) {// 灰度 mat
            cvtColor(mat, temp, COLOR_GRAY2BGRA);
        }
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {// C2
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        if (mat.type() == CV_8UC4) {
            cvtColor(mat, temp, COLOR_BGRA2BGR565);
        } else if (mat.type() == CV_8UC2) {
            mat.copyTo(temp);

        } else if (mat.type() == CV_8UC1) {// 灰度 mat
            cvtColor(mat, temp, COLOR_GRAY2BGR565);
        }
    }
    // 其他要自己去转

    // 解锁 Bitmap 画布
    AndroidBitmap_unlockPixels(env, bitmap);
}

CascadeClassifier cascadeClassifier;
extern "C" JNIEXPORT void JNICALL
Java_com_shixin_pratice_1opencv_FaceDetection_loadCascade(JNIEnv *env, jobject instance,
                                                          jstring filePath_) {
    const char *filePath = env->GetStringUTFChars(filePath_, 0);
    cascadeClassifier.load(filePath);
    LOGE("加载分类器文件成功");
    env->ReleaseStringUTFChars(filePath_, filePath);
}

extern "C" JNIEXPORT jint JNICALL
Java_com_shixin_pratice_1opencv_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject instance,
                                                                    jobject bitmap) {
    // 检测人脸  , opencv 有一个非常关键的类是 Mat ，opencv 是 C 和 C++ 写的，只会处理 Mat , android里面是Bitmap
    // 1. Bitmap 转成 opencv 能操作的 C++ 对象 Mat , Mat 是一个矩阵
    Mat mat;
    bitmap2Mat(env, mat, bitmap);

    // 处理灰度 opencv 处理灰度图, 提高效率，一般所有的操作都会对其进行灰度处理 高斯模糊
    Mat gray_mat;
    cvtColor(mat, gray_mat, COLOR_BGRA2GRAY);

    // 再次处理 直方均衡补偿
    Mat equalize_mat;
    equalizeHist(gray_mat, equalize_mat);

    // 识别人脸，当然我们可以直接用 彩色图去做,识别人脸要加载人脸分类器文件
    std::vector<Rect> faces;
    cascadeClassifier.detectMultiScale(equalize_mat, faces, 1.1, 5);
    LOGE("人脸个数：%d", faces.size());
    if (faces.size() == 1) {
        Rect faceRect = faces[0];

        // 在人脸部分花个图
        rectangle(mat, faceRect, Scalar(255, 155, 155), 2);
        // 把 mat 我们又放到 bitmap 里面
        mat2Bitmap(env, mat, bitmap);
        // 保存人脸信息 Mat , 图片 jpg
        Mat face_info_mat(equalize_mat, faceRect);
        // 保存 face_info_mat
    }

    // 保存人脸信息
    return 0;
}



extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_againstWorld(JNIEnv *env, jclass clazz,
                                                            jobject bitmap) {
    //bitmap-> mat
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat res(src.size(), src.type());
    //获取图片的宽高
    int src_w = src.cols;
    int src_h = src.rows;
    int mid_h = src_h >> 1;
    int a_h = mid_h >> 1;

    //处理下半部分
    for (int rows = 0; rows < mid_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            res.at<Vec4b>(rows + mid_h, cols) = src.at<Vec4b>(rows + a_h, cols);
        }
    }
    for (int rows = 0; rows < mid_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            res.at<Vec4b>(rows, cols) = src.at<Vec4b>(src_h - a_h - rows, cols);
        }
    }
    cv_helper::mat2bitmap(env, res, bitmap);
    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_anaglyph(JNIEnv *env, jclass clazz, jobject bitmap) {
    // 有立体感，突出了轮廓信息，opencv  卷积
    // [1,0]
    // [0,-1]
    // bitmap -> mat
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat res(src.size(), src.type());
    // 获取图片宽高
    int src_w = src.cols;
    int src_h = src.rows;

    for (int rows = 0; rows < src_h - 1; ++rows) {
        for (int cols = 0; cols < src_w - 1; ++cols) {
            Vec4b pixels_p = src.at<Vec4b>(rows, cols);
            Vec4b pixels_n = src.at<Vec4b>(rows + 1, cols + 1);
            // bgra
            res.at<Vec4b>(rows, cols)[0] = saturate_cast<uchar>(pixels_p[0] - pixels_n[0] + 128);
            res.at<Vec4b>(rows, cols)[1] = saturate_cast<uchar>(pixels_p[1] - pixels_n[1] + 128);
            res.at<Vec4b>(rows, cols)[2] = saturate_cast<uchar>(pixels_p[2] - pixels_n[2] + 128);
        }
    }

    cv_helper::mat2bitmap(env, res, bitmap);

    return bitmap;
}




extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_groundGlass(JNIEnv *env, jclass clazz,
                                                           jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // 高斯模糊，毛玻璃 （对某个区域取随机像素）
    int src_w = src.cols;
    int src_h = src.rows;
    int size = 8;
    // 知识不是用来背的 20% ，用来唤醒大家的
    RNG rng(time(NULL));
    for (int rows = 0; rows < src_h - size; ++rows) {
        for (int cols = 0; cols < src_w - size; ++cols) {
            int random = rng.uniform(0, 8);
            src.at<int>(rows, cols) = src.at<int>(rows + random, cols + random);
        }
    }
    cv_helper::mat2bitmap(env, src, bitmap);
    return bitmap;

}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_mosaic(JNIEnv *env, jclass clazz, jobject bitmap) {

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    int src_h = src.rows;
    int src_w = src.cols;

    int rows_s = src_h >> 2;
    int rows_e = src_h * 3 / 4;
    int cols_s = src_w >> 2;
    int cols_e = src_w * 3 / 4;
    int size = 10;
    for (int rows = rows_s; rows < rows_e; rows += size) {
        for (int cols = cols_s; cols < cols_e; cols += size) {
            int pixels = src.at<int>(rows, cols);
            for (int m_rows = 1; m_rows < size; ++m_rows) {
                for (int m_cols = 1; m_cols < size; ++m_cols) {
                    src.at<int>(rows + m_rows, cols + m_cols) = pixels;
                }
            }
        }
    }

    cv_helper::mat2bitmap(env, src, bitmap);
    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_oilPainting(JNIEnv *env, jclass clazz,
                                                           jobject bitmap) {
    // 油画基于直方统计
    // 1. 每个点需要分成 n*n 小块
    // 2. 统计灰度等级
    // 3. 选择灰度等级中最多的值
    // 4. 找到所有的像素取平均值

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    Mat gary;
    cvtColor(src, gary, COLOR_BGRA2GRAY);
    Mat res(src.size(), src.type());

    int src_w = src.cols;
    int src_h = src.rows;

    int size = 8;

    /*for (int rows = 0; rows < src_h - size; ++rows) {
        for (int cols = 0; cols < src_w - size; ++cols) {
            int g[8] = {0}, b_g[8] = {0}, g_g[8] = {0}, r_g[8] = {0};            //255 / 8
            // 这个位置  64 循环 -> 1 个像素点 ， 高斯模糊 ，想想怎么优化
            for (int o_rows = 0; o_rows < size; ++o_rows) {
                for (int o_cols = 0; o_cols < size; ++o_cols) {
                    uchar gery = gary.at<uchar>(rows + o_rows, cols + o_cols);
                    uchar index = gery / (254 / 7); // 254*8/254
                    g[index] += 1;
                    //等级的像素值之和
                    b_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[0];
                    g_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[1];
                    r_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[2];

                }
            }

            //最大的角标找出来
            int max_index = 0;
            int max = g[0];
            for (int i = 0; i < size; ++i) {
                if (g[max_index] < g[i]) {
                    max_index = i;
                    max = g[i];
                }
            }

            res.at<Vec4b>(rows, cols)[0] = b_g[max_index] / max;
            res.at<Vec4b>(rows, cols)[1] = g_g[max_index] / max;
            res.at<Vec4b>(rows, cols)[2] = r_g[max_index] / max;
        }
    }*/
    // 知识不是用来背的 20% ，用来唤醒大家的
    for (int rows = 0; rows < src_h - size; ++rows) {
        for (int cols = 0; cols < src_w - size; ++cols) {
            int g[8] = {0}, b_g[8] = {0}, g_g[8] = {0}, r_g[8] = {0};// 255/8
            // 这个位置  64 循环 -> 1 个像素点 ， 高斯模糊 ，想想怎么优化
            for (int o_rows = 0; o_rows < size; ++o_rows) {
                for (int o_cols = 0; o_cols < size; ++o_cols) {
                    uchar gery = gary.at<uchar>(rows + o_rows, cols + o_cols);
                    uchar index = gery / (254 / 7); // 254*8/254
                    g[index] += 1;
                    // 等级的像素值之和
                    b_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[0];
                    g_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[1];
                    r_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[2];
                }
            }
            // 最大的角标找出来
            int max_index = 0;
            int max = g[0];
            for (int i = 1; i < size; ++i) {
                if (g[max_index] < g[i]) {
                    max_index = i;
                    max = g[i];
                }
            }
            // 会超过 255 ， 会不会超过，超过了会怎样 （头条面试）  截取掉最高位
            res.at<Vec4b>(rows, cols)[0] = b_g[max_index] / max;
            res.at<Vec4b>(rows, cols)[1] = g_g[max_index] / max;
            res.at<Vec4b>(rows, cols)[2] = r_g[max_index] / max;
        }
    }


    cv_helper::mat2bitmap(env, res, bitmap);
    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_garyOptimize(JNIEnv *env, jclass clazz,
                                                            jobject bitmap) {

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // 高斯模糊，毛玻璃 （对某个区域取随机像素）
    int src_w = src.cols;
    int src_h = src.rows;
    // 知识不是用来背的 20% ，用来唤醒大家的
    for (int rows = 0; rows < src_h; ++rows) {
        for (int cols = 0; cols < src_w; ++cols) {
            Vec4b pixels = src.at<Vec4b>(rows, cols);
            uchar b = pixels[0];
            uchar g = pixels[1];
            uchar r = pixels[2];
            // 好几种算法求灰度
            // uchar gery = (b + g + r) / 3;
            // 位移 > +- >  */  int > float
            // uchar gery = 0.299 * r + 0.587 * g + 0.1114 * b;
            uchar gery = (r + (g << 1) + b) >> 2; // 0.4  sobel  |x|+|y|
            src.at<Vec4b>(rows, cols)[0] = gery;
            src.at<Vec4b>(rows, cols)[1] = gery;
            src.at<Vec4b>(rows, cols)[2] = gery;
        }
    }
    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_blur(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    Mat dst;
    // Size(w,h) 只能是奇数，这样才能找到中心点，模糊核的大小
    //kernel  越大越模糊
    blur(src, dst, Size(9, 9), Point(-1, -1));

    cv_helper::mat2bitmap(env, dst, bitmap);

    return bitmap;
}


// 直方均衡，提升对比度
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_hsv(JNIEnv *env, jclass clazz, jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    Mat hsv;
    cvtColor(src, hsv, COLOR_BGR2HSV);

    std::vector<Mat> hsv_s;
    split(hsv, hsv_s);

    equalizeHist(hsv_s[2], hsv_s[2]);

    merge(hsv_s, hsv);
    cvtColor(hsv, src, COLOR_HSV2BGR);

    cv_helper::mat2bitmap(env, src, bitmap);

    return bitmap;

}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_filter2D(JNIEnv *env, jclass clazz, jobject bitmap) {

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    // 高斯（模糊），计算高斯卷积核，卷积操作，考虑像素之间的差值（更好的保留图像的边缘）
    // 2-3 S
    Mat dst;

    int size = 55;
    Mat kernel = Mat::ones(size, size, CV_32FC1) / (size * size);

    long st = getTickCount();
    filter2D(src, dst, src.depth(), kernel);
    long et = getTickCount();
    long time = (et - st) * 1000 / getTickFrequency();
    std::cout << "time" << time << std::endl;

    // 时间复杂度问题
    // 时间复杂度 = src.rows * src.cols * size * size
    // 小的作业：手写 filter2D 的源码

    cv_helper::mat2bitmap(env, dst, bitmap);

    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_bilateralFilter(JNIEnv *env, jclass clazz,
                                                               jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    // 高斯（模糊），计算高斯卷积核，卷积操作，考虑像素之间的差值（更好的保留图像的边缘）
    // 2-3 S
    Mat dst;
    bilateralFilter(src, dst, 0, 100, 10);


    cv_helper::mat2bitmap(env, dst, bitmap);

    return bitmap;
}


Mat imgOriginal, imgWarp, imgCrop;;

void drawPoints(vector<Point> points, Scalar color) {
    for (int i = 0; i < points.size(); i++) {
        circle(imgOriginal, points[i], 10, color, FILLED);
        putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
    }
}

//重新排序边缘
vector<Point> reorder(vector<Point> points) {
    vector<Point> newPoints;
    vector<int> sumPoints, subPoints;

    for (int i = 0; i < 4; i++) {
        sumPoints.push_back(points[i].x + points[i].y);
        subPoints.push_back(points[i].x - points[i].y);
    }
    //cout << sumPoints.begin() << endl;
    newPoints.push_back(
            points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); // 0
    //cout << newPoints[0].x << endl;
    newPoints.push_back(
            points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //1
    newPoints.push_back(
            points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //2
    newPoints.push_back(
            points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //3
    cout << sizeof(newPoints) << endl;
    return newPoints;
}

vector<Point> initialPoints, docPoints;
float w = 3264, h = 7252;

Mat getWarp(Mat img, vector<Point> points, float w, float h) {
    Point2f src[4] = {points[0], points[1], points[2], points[3]};
    Point2f dst[4] = {{0.0f, 0.0f},
                      {w,    0.0f},
                      {0.0f, h},
                      {w,    h}};
    Mat matrix = getPerspectiveTransform(src, dst);
    warpPerspective(img, imgWarp, matrix, Point(w, h));
    return imgWarp;
}

void drawPoints(vector<Point> points, Scalar color, Mat imgOriginal) {
    for (int i = 0; i < points.size(); i++) {
        circle(imgOriginal, points[i], 10, color, FILLED);
        putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
    }
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_findContours(JNIEnv *env, jclass clazz,
                                                            jobject bitmap) {

    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);

    //resize(src, src, Size(), 0.5, 0.5);

    Mat gary;
    cvtColor(src, gary, COLOR_BGRA2GRAY);

    //模糊
    Mat imgBlur;
    GaussianBlur(gary, imgBlur, Size(3, 3), 3, 0);


    // 梯度和二值化
    Mat imgCanny;
    Canny(imgBlur, imgCanny, 25, 75);

    // 膨胀
    Mat imgDil;

    //卷积核
    Mat kernel = getStructuringElement(MORPH_RECT, Size(35, 35));
    dilate(imgCanny, imgDil, kernel);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    //查找边缘
    findContours(imgDil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    vector<Point> biggest;
    int maxArea = 0;

    //提取并标记文档边缘
    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        //cout << area << endl;
        string objectType;

        if (area > 1000000) {
            LOGE("area=====>%d", area);

            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            if (area > maxArea && conPoly[i].size() == 4) {

                //绘制识别到的边界
                drawContours(src, conPoly, i, Scalar(0, 0, 0), 10);
                biggest = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
                maxArea = area;
            }
            //绘制面积大于1000000平方的边界。
            // drawContours(src, conPoly, i, Scalar(0, 0, 0), 10);
            // rectangle(src, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 5);
        }
    }

    LOGE("biggest，%d", biggest.size());

    docPoints = reorder(biggest);
    LOGE("数量，%d", docPoints.size());
    drawPoints(docPoints, Scalar(0, 255, 0), src);

    imgWarp = getWarp(src, docPoints, w, h);
    LOGE("imgWarp.rows，%d", imgWarp.rows);
    LOGE("imgWarp.cols，%d", imgWarp.cols);
    int cropVal = 5;
    Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
    imgCrop = imgWarp(roi);

    LOGE("imgCrop.rows，%d", imgCrop.rows);
    LOGE("imgCrop.cols，%d", imgCrop.cols);


    /* Mat contours_mat = Mat::zeros(src.size(), CV_8UC3);
     Rect card_rect;
     for (int i = 0; i < contours.size(); i++) {
         // 画轮廓
         Rect rect = boundingRect(contours[i]);
         //筛选轮廓
         if (rect.width > src.cols / 20 && rect.height > src.rows / 20) {
             drawContours(contours_mat, contours, i, Scalar(0, 0, 255), 1);
             card_rect = rect;
             rectangle(contours_mat, Point(rect.x, rect.y),
                       Point(rect.x + rect.width, rect.y + rect.height),
                       Scalar(255, 255, 255), 2);
             break;
         }
     }*/


    jobject bitmap1 = cv_helper::createBitMap(env, w, h, 0);
    cv_helper::mat2bitmap(env, imgCrop, bitmap1);
    return bitmap1;
}

extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_matToBitmap(JNIEnv *env, jclass clazz,
                                                           jlong native_obj) {
    Mat *src = reinterpret_cast<Mat *>(native_obj);

    //resize(src, src, Size(), 0.5, 0.5);

    Mat gary;
    cvtColor(*src, gary, COLOR_BGRA2GRAY);

    //模糊
    Mat imgBlur;
    GaussianBlur(gary, imgBlur, Size(3, 3), 3, 0);


    // 梯度和二值化
    Mat imgCanny;
    Canny(imgBlur, imgCanny, 25, 75);

    // 膨胀
    Mat imgDil;

    //卷积核
    Mat kernel = getStructuringElement(MORPH_RECT, Size(35, 35));
    dilate(imgCanny, imgDil, kernel);


    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    //查找边缘
    findContours(imgDil, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);

    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());

    vector<Point> biggest;
    int maxArea = 0;

    //提取并标记文档边缘
    for (int i = 0; i < contours.size(); i++) {
        int area = contourArea(contours[i]);
        //cout << area << endl;
        string objectType;

        if (area > 1000000) {
            LOGE("area=====>%d", area);

            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02 * peri, true);

            if (area > maxArea && conPoly[i].size() == 4) {

                //绘制识别到的边界
                drawContours(*src, conPoly, i, Scalar(0, 0, 0), 10);
                biggest = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
                maxArea = area;
            }
            //绘制面积大于1000000平方的边界。
            // drawContours(src, conPoly, i, Scalar(0, 0, 0), 10);
            // rectangle(src, boundRect[i].tl(), boundRect[i].br(), Scalar(0, 0, 255), 5);
        }
    }

    LOGE("biggest，%d", biggest.size());

    docPoints = reorder(biggest);
    LOGE("数量，%d", docPoints.size());
    drawPoints(docPoints, Scalar(0, 255, 0), *src);

    imgWarp = getWarp(*src, docPoints, w, h);
    LOGE("imgWarp.rows，%d", imgWarp.rows);
    LOGE("imgWarp.cols，%d", imgWarp.cols);
    int cropVal = 5;
    Rect roi(cropVal, cropVal, w - (2 * cropVal), h - (2 * cropVal));
    imgCrop = imgWarp(roi);

    LOGE("imgCrop.rows，%d", imgCrop.rows);
    LOGE("imgCrop.cols，%d", imgCrop.cols);


    /* Mat contours_mat = Mat::zeros(src.size(), CV_8UC3);
     Rect card_rect;
     for (int i = 0; i < contours.size(); i++) {
         // 画轮廓
         Rect rect = boundingRect(contours[i]);
         //筛选轮廓
         if (rect.width > src.cols / 20 && rect.height > src.rows / 20) {
             drawContours(contours_mat, contours, i, Scalar(0, 0, 255), 1);
             card_rect = rect;
             rectangle(contours_mat, Point(rect.x, rect.y),
                       Point(rect.x + rect.width, rect.y + rect.height),
                       Scalar(255, 255, 255), 2);
             break;
         }
     }*/


    jobject bitmap1 = cv_helper::createBitMap(env, w, h, 0);
    cv_helper::mat2bitmap(env, imgCrop, bitmap1);
    return bitmap1;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_shixin_pratice_1opencv_NdkBitmapUtils_removeShadows(JNIEnv *env, jclass clazz,
                                                             jobject bitmap) {
    Mat src;
    cv_helper::bitmap2mat(env, bitmap, src);
    // CvUtils::MatResize(src, 800, 520);
    //imshow("src", src);


    //1.将图像转为灰度图
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    //CvUtils::SetShowWindow(gray, "gray", 0, 30);
    //imshow("gray", gray);


    //定义腐蚀和膨胀的结构化元素和迭代次数
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    int iteration = 9;


    //2.将灰度图进行膨胀操作
    Mat dilateMat;
    morphologyEx(gray, dilateMat, MORPH_DILATE, element, Point(-1, -1), iteration);
    //imshow("dilate", dilateMat);


    //3.将膨胀后的图再进行腐蚀
    Mat erodeMat;
    morphologyEx(dilateMat, erodeMat, MORPH_ERODE, element, cv::Point(-1, -1), iteration);
    //imshow("erode", erodeMat);


    //4.膨胀再腐蚀后的图减去原灰度图再进行取反操作
    Mat calcMat = ~(erodeMat - gray);
    //CvUtils::SetShowWindow(calcMat, "calc", gray.cols*2, 30);
   // imshow("calc", calcMat);


    //5.使用规一化将原来背景白色的改了和原来灰度图差不多的灰色
    Mat removeShadowMat;
    normalize(calcMat, removeShadowMat, 0, 200, NORM_MINMAX);


}