//
// Created by shixin on 2022/9/18.
//
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;

void teststd(){

}

/*
#define MAX(a,b) a>b ? a:b
// inline 内联函数，类似于 define 函数，做到编译时替换
// 区别：define 并没有严格的检测
// inline 使用场景：不涉及过于复杂的函数
 // 运行时被替换
inline int max(int a, int b){
	return a > b ? a : b;
}

 int main(){

	Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");

	// 判断读取是否正确，文件不存在
	if (src.empty()){
		cout << " src imread error " << endl;
		return -1;
	}

	// 2. 绘制形状和文字
	// 2.1 线 line  LINE_8 LINE_4 LINE_AA 的区别
	line(src, Point(100, 100), Point(200, 200),Scalar(255,0,0), 2, LINE_8);

	// 矩形 rectangle
	rectangle(src,Point(100,100),Point(300,300),Scalar(0,0,255),2,LINE_8);

	// 椭圆 ellipse
	// 第二个参数是： 椭圆的中心点
	// 第三个参数是： Size 第一个值是椭圆 x width 的半径 ，第二个 ...
	//
	// ellipse(src, Point(src.cols / 2, src.rows / 2), Size(src.cols / 8, src.rows / 4), 180, 180, 360
	//	,Scalar(0, 255, 255), 2);

	// 圆 cicle LINE_AA 抗锯齿
	// circle(src, Point(src.cols / 2, src.rows / 2), src.rows / 4,Scalar(255,255,0),2,LINE_AA);

	// 填充 fillPoly 多边形  inputArray mat矩阵的数组
	// CV_EXPORTS void fillPoly(Mat& img,
    //   const Point** pts,
	//	 const int* npts,
    //   int ncontours,
	// 	 const Scalar& color, int lineType = LINE_8, int shift = 0,
	// 	 Point offset = Point());

    // 二维数组  三角形
	Point pts[1][4];
	pts[0][0] = Point(100, 100);
	pts[0][1] = Point(100, 200);
	pts[0][2] = Point(200, 200);
	pts[0][3] = Point(100, 100);

    //类型是const
	const Point* ptss[] = { pts[0] };

	const int npts[] = { 4 };

	fillPoly(src, ptss, npts, 1, Scalar(255, 0, 0), 8);

	// 文字 putText
	// fontFace 字体种类
	// fontScale 1 缩放   左上角
	putText(src, "Hello OpenCV", Point(100, 100), CV_FONT_BLACK, 1, Scalar(0, 0, 255),1, LINE_AA);

	// 随机画 srand 画线
	// opencv 做随机 srand random 效果一样
	RNG rng(time(NULL));

	// 创建一张图，与 src 的宽高和颜色通道一致 ，所有的点都是 0，黑色的
	Mat dst = Mat::zeros(src.size(),src.type());

	for (int i = 0; i < 1000; i++){
		Point sp;
		sp.x = rng.uniform(0, dst.cols);
		sp.y = rng.uniform(0, dst.rows);
		Point ep;
		ep.x = rng.uniform(0, dst.cols);
		ep.y = rng.uniform(0, dst.rows);
		line(dst, sp, ep, Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)));
	}

	imshow("src",dst);

	waitKey(0);

	getchar();
}
*/

//4. 模糊图像，都是针对每个像素点进行处理
//根据周边的像素值来确定自己的像素值，平均值，最大值，最小值，正太分布值
//4.1 均值模糊 blur
//
//
//4.2 高斯模糊 GaussianBlur
//如果要去写高斯模糊的效果或者说均值模糊的效果，
// Java 来写思路：传入一个半径，根据半径计算出 权值，然后相乘 半径内的像素点，赋值给自己。
// 越靠近的点权重越高，然后计算出加权的的值

// opencv处理的方式，在图像卷积的基础上去做的处理。

// 图像的卷积和滤波操作
// 图像矩阵掩膜操作

// 定义一个矩阵，相当于覆盖到原来的图上，被称为掩膜
 // [ -1 0, 1
//    1, 0, 1
//    0, 0, 0]
// 和图中取出相等大小的矩阵计算(某种计算公式)，计算值后赋值给中心点
// 这个过程被称为卷积操作。

// 4.3 中值滤波   降噪
// 4.4 双边模糊   美图
//int mainx(){
//
//    Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");
//
//    // 判断读取是否正确，文件不存在
//    if (src.empty()){
//        cout << " src imread error " << endl;
//        return -1;
//    }
//
//    /*
//    //生成一个和源图像大小相等类型相同的全0矩阵
//    Mat dest = Mat::zeros(src.size(), src.type());
//
//    //获取图像的列数,一定不要忘记图像的通道数，需要rgb值
//    int cols = (src.cols - 1) * src.channels();
//    int rows = src.rows;//获取图像的行数
//
//     // 通道数
//    int offsetx = src.channels();
//
//    for (int row = 1; row < rows - 1; row++){
//        // 上一行
//        uchar* previous = src.ptr<uchar>(row - 1);
//        // 当前行
//        uchar* current = src.ptr<uchar>(row);
//        // 下一行
//        uchar* next = src.ptr<uchar>(row + 1);
//        // 输出
//        uchar* output = dest.ptr<uchar>(row);
//        for (int col = offsetx; col < cols; col++){
//            //  5 * current[col] 自己
//            // (current[col - offsetx]  左侧  偏移量是通道数，横向的rgb，rgb排列
//            // current[col + offsetx]  右侧
//            // previous[col] 上一行
//            // next[col]  下一行
//            output[col] = saturate_cast<uchar>(
//                5 * current[col] - (current[col - offsetx] + current[col + offsetx] + previous[col] + next[col]));
//        }
//    }
//    */
//
//    // 再去查查
//    Mat dest;
//    // src depth 下周再讲，Point(-1, -1) 中心点
//    Mat kernel = (Mat_<char>(3,3)<< 0, -1, 0, -1, 5, -1, 0, -1, 0);
//    // filter2D(src, dest, src.depth(), kernel);
//
//    // 调用filter2D，高斯模糊,定义一个掩膜
//    filter2D(src, dest, src.depth(), kernel);
//
//    imshow("src", dest);
//   // line(src, Point(100, 100), Point(200, 200),Scalar(255,0,0), 2, LINE_8);
//
//    //fillPoly()
//    waitKey(0);
//    return 0;
//}


//均值模糊，和高斯模糊
//int main1(){
//
//    Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");
//
//    if (!src.data){
//        printf("imread error!");
//        return -1;
//    }
//
//    imshow("src", src);
//
//    // 均值模糊
//    Mat dst;
//    // Size(w,h) 只能是奇数，这样才能找到中心点，+模糊核的大小
//    blur(src, dst, Size(151, 151), Point(-1,-1));
//    imshow("blur", dst);
//
//    // sigmaX sigmaY 代表 作用
//    // sigmaY 不传代表和 sigmaX 一样
//    // 如果 sigmaX >= 0 ,自己会计算 0.3*((ksize-1)*0.5 - 1) + 0.8
//    // 自己传得怎么传？有什么意义 1 ，
//    Mat gaussian;
//    GaussianBlur(src, gaussian, Size(151, 151),0);
//    imshow("gaussian", gaussian);
//
//    waitKey(0);
//    return 0;
//}
