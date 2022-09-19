#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;
using namespace std;

/* 电脑端操作、
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test3.png");

	// 判断读取是否正确，文件不存在
	if (src.empty()){
		cout << " src imread error " << endl;
		return -1;
	}
	// uchar* data; 头指针
	if (!src.data){
		cout << " src imread error " << endl;
		return -1;
	}

	// imshow("src", src);

	// 获取信息 
	int cols = src.cols;// 宽
	int rows = src.rows;// 高
	int chnnales = src.channels();

	// 创建 CV_8UC1 一个颜色的通道 （0-255） 
	// CV_8UC2      2个颜色的通道   16位  RGB565
	// CV_8UC3      3个颜色的通道   24位
	// CV_8UC4      4个颜色的通道   32位  ARGB8888
    //
	// 匹配上 Java Bitmap 的颜色通道  RGB565   ARGB8888  
	// Scalar 指定颜色   0是黑色
	// 第一个参数 rows 高，第二参数是 cols 宽
    //创建一个矩阵  3x3矩阵
	// Mat mat(3,3,CV_8UC1,Scalar(255));

	// Size 第一个参数是 width ，第二个参数是 height
	// Mat mat(Size(3,3), CV_8UC1, Scalar(255));



	// 三颜色通道
	// Mat test(Size(3, 3), CV_8UC3, Scalar(255,0,255));// BGR 

	// cout << mat << endl;

	// 拷贝构造函数
	// Mat mat(src);
	// Mat mat = src; 不会去拷贝内容的

	Mat mat;
	// 会去拷贝内容
	// src.copyTo(mat);
	mat = src.clone();

	imshow("mat", mat);

	cout << cols << "," << rows << "," << chnnales;

	waitKey(0);
	return 0;
}

// 2. 图像操作
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");


	// 回顾前几次讲的，通过像素指针去获取修改
	
	// 2.3 区域截取 (不涉及创建新的内容),指向同一块首地址
	Mat srcROI = src(Rect(20,20,400,400));

    //原图
	//int rows = srcROI.rows;
	//int cols = srcROI.cols;


	// 不改变原图，克隆出一份
	Mat dstROI = srcROI.clone();

	int rows = dstROI.rows;
	int cols = dstROI.cols;

    //改变其中的一块区域
	// inline 内联函数 查查资料
    //-------------------------
    //----- at() 方法 内联方法
    //-------------------------
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{	
			// 获取像素 at()  Vec3b 个参数
			int b = dstROI.at<Vec3b>(i, j)[0];
			int g = dstROI.at<Vec3b>(i, j)[1];
			int r = dstROI.at<Vec3b>(i, j)[2];

			// 修改像素 (底片效果)
			dstROI.at<Vec3b>(i, j)[0] = 255 - b;
			dstROI.at<Vec3b>(i, j)[1] = 255 - g;
			dstROI.at<Vec3b>(i, j)[2] = 255 - r;
		}
	}


    //转换为灰度图
	Mat gary;
	cvtColor(src, gary, COLOR_BGR2GRAY);
	// 获取信息 
	int cols = gary.cols;// 宽
	int rows = gary.rows;// 高
	int chnnales = gary.channels();// 通道是1 ，压缩量更加的大
	cout << cols << "," << rows << "," << chnnales;

	// Bitmap 里面转的是 4 通道 ， 一个通道就可以代表灰度
    // 自己转是4通道
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
		//需要判断通道
			if (chnnales == 3){
				// 获取像素 at  Vec3b 个参数
				int b = gary.at<Vec3b>(i, j)[0];
				int g = gary.at<Vec3b>(i, j)[1];
				int r = gary.at<Vec3b>(i, j)[2];

				// 修改像素 (底片效果)
				gary.at<Vec3b>(i, j)[0] = 255 - b;
				gary.at<Vec3b>(i, j)[1] = 255 - g;
				gary.at<Vec3b>(i, j)[2] = 255 - r;
			}else if (chnnales == 1){
				uchar pixels = gary.at<uchar>(i, j);
				gary.at<uchar>(i, j) = 255 - pixels;
			}
		}
	}
	


    //手动转灰度图
	// 获取信息 
	int cols = src.cols;// 宽
	int rows = src.rows;// 高
	int chnnales = src.channels();// 1 

    //重新定义gray矩阵
	Mat gary(rows,cols,CV_8UC1);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (chnnales == 3){
				// 获取像素 at  Vec3b 个参数
				int b = src.at<Vec3b>(i, j)[0];
				int g = src.at<Vec3b>(i, j)[1];
				int r = src.at<Vec3b>(i, j)[2];

				// = ? 
				gary.at<uchar>(i, j) = 0.11f*r + 0.59*g+ 0.30*b;
			}
		}
	}

	// cvtColor(src, gary, COLOR_BGR2GRAY);

	// 显示 src 这张图是什么效果？
	imwrite("C:/Users/hcDarren/Desktop/android/gary.png", gary);
	imshow("gary", gary);
	waitKey(0);
	return 0;
}


// 图片加水印？图片混合  （自己操作指针去实现）方法
 //void addWeighted(InputArray src1, double alpha, InputArray src2, double beta, double gamma, OutputArray dst, int dtype=-1); 第一个参数，InputArray类型的src1，表示需要加权的第一个数组，常常填一个Mat。
 // 第一个参数，src1第一个数组
 //第二个参数，alpha，表示第一个数组的权重
 //第三个参数，src2，表示第二个数组，它需要和第一个数组拥有相同的尺寸和通道数。
 //第四个参数，beta，表示第二个数组的权重值。
 //第五个参数，dst，输出的数组，它和输入的两个数组拥有相同的尺寸和通道数。
 //第六个参数，gamma，一个加到权重总和上的标量值。
 //第七个参数，dtype，输出阵列的可选深度，有默认值-1。;当两个输入数组具有相同的深度时，这个参数设置为-1（默认值），即等同于src1.depth（）
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");
	if (src.empty()){
		cout << " src imread error " << endl;
		return -1;
	}
	
	Mat logo = imread("C:/Users/hcDarren/Desktop/android/test1.png");
	if (logo.empty()){
		cout << " logo imread error " << endl;
		return -1;
	}
	

	Mat logo = imread("C:/Users/hcDarren/Desktop/android/android_logo.jpg");
	// 注意两张图片的大小必须得一致
	Mat dst;
	// 非常生硬，直接像素相加
	// add(src,logo,dst);
	//  alpha + beta 的值可以大于1
    //saturate_cast()  这个函数转换数值从0-255
	// dst(x) = saturate_cast(src(x)*alpha + logo(x)*beta + gamma) 
	// addWeighted(src,0.8, logo, 0.2, 0.0, dst);

	// 非得加一个 logo 上去怎么加？思路能想到
    // 截取出大小和logo一致，然后两个叠加
	Mat srcROI = src(Rect(0, 0, logo.cols, logo.rows));
	// 并不适合去加水印，只适合做图片混合
	addWeighted(srcROI, 0.8, logo, 1, 0.0, srcROI);

	// 作业完成：真正的加水印

	imshow("dst", src);
	waitKey(0);
	return 0;
}

// 4. 饱和度、亮度和对比度
// 饱和度：图片更加饱和是什么意思？ 红色，淡红，鲜红，红得发紫等等   R -> 1.2R B -> 1.2B G -> 1.2G
// 亮度：亮暗的程度
// 对比度: 图像对比度指的是 [1]  一幅图像中明暗区域最亮的白和最暗的黑之间不同亮度层级的测量，即指一幅图像灰度反差的大小
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");
	if (src.empty()){
		cout << " src imread error " << endl;
		return -1;
	}

	// 滤镜 UI设计师 再调亮一点 ，饱和一点，对比度再调高一点？
    // alpha 增大成比例去增 1:500  10:5000  beta增
    //
	// 通过 alpha 可以调整 饱和度 , 对比度
	// beta 亮度
	// F(R) = alpha*R + beta;
	// F(G) = alpha*G + beta;
	// F(B) = alpha*B + beta;
	// 获取信息 
	int cols = src.cols;// 宽
	int rows = src.rows;// 高
	int chnnales = src.channels();// 1 

	int alpha = 1;
	int beta = -50;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (chnnales == 3){
				// 获取像素 at  Vec3b 个参数
				int b = src.at<Vec3b>(i, j)[0];
				int g = src.at<Vec3b>(i, j)[1];
				int r = src.at<Vec3b>(i, j)[2];


				src.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(b*alpha + beta);
				src.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(g*alpha + beta);
				src.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(r*alpha + beta);
			}
		}
	}

	imshow("src", src);
	waitKey(0);

	return 0;
}
*/
