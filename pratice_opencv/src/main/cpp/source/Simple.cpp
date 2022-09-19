#include <opencv2/opencv.hpp>

#include <iostream>

using namespace cv;
using namespace std;

/* ���Զ˲�����
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test3.png");

	// �ж϶�ȡ�Ƿ���ȷ���ļ�������
	if (src.empty()){
		cout << " src imread error " << endl;
		return -1;
	}
	// uchar* data; ͷָ��
	if (!src.data){
		cout << " src imread error " << endl;
		return -1;
	}

	// imshow("src", src);

	// ��ȡ��Ϣ 
	int cols = src.cols;// ��
	int rows = src.rows;// ��
	int chnnales = src.channels();

	// ���� CV_8UC1 һ����ɫ��ͨ�� ��0-255�� 
	// CV_8UC2      2����ɫ��ͨ��   16λ  RGB565
	// CV_8UC3      3����ɫ��ͨ��   24λ
	// CV_8UC4      4����ɫ��ͨ��   32λ  ARGB8888
    //
	// ƥ���� Java Bitmap ����ɫͨ��  RGB565   ARGB8888  
	// Scalar ָ����ɫ   0�Ǻ�ɫ
	// ��һ������ rows �ߣ��ڶ������� cols ��
    //����һ������  3x3����
	// Mat mat(3,3,CV_8UC1,Scalar(255));

	// Size ��һ�������� width ���ڶ��������� height
	// Mat mat(Size(3,3), CV_8UC1, Scalar(255));



	// ����ɫͨ��
	// Mat test(Size(3, 3), CV_8UC3, Scalar(255,0,255));// BGR 

	// cout << mat << endl;

	// �������캯��
	// Mat mat(src);
	// Mat mat = src; ����ȥ�������ݵ�

	Mat mat;
	// ��ȥ��������
	// src.copyTo(mat);
	mat = src.clone();

	imshow("mat", mat);

	cout << cols << "," << rows << "," << chnnales;

	waitKey(0);
	return 0;
}

// 2. ͼ�����
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");


	// �ع�ǰ���ν��ģ�ͨ������ָ��ȥ��ȡ�޸�
	
	// 2.3 �����ȡ (���漰�����µ�����),ָ��ͬһ���׵�ַ
	Mat srcROI = src(Rect(20,20,400,400));

    //ԭͼ
	//int rows = srcROI.rows;
	//int cols = srcROI.cols;


	// ���ı�ԭͼ����¡��һ��
	Mat dstROI = srcROI.clone();

	int rows = dstROI.rows;
	int cols = dstROI.cols;

    //�ı����е�һ������
	// inline �������� �������
    //-------------------------
    //----- at() ���� ��������
    //-------------------------
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{	
			// ��ȡ���� at()  Vec3b ������
			int b = dstROI.at<Vec3b>(i, j)[0];
			int g = dstROI.at<Vec3b>(i, j)[1];
			int r = dstROI.at<Vec3b>(i, j)[2];

			// �޸����� (��ƬЧ��)
			dstROI.at<Vec3b>(i, j)[0] = 255 - b;
			dstROI.at<Vec3b>(i, j)[1] = 255 - g;
			dstROI.at<Vec3b>(i, j)[2] = 255 - r;
		}
	}


    //ת��Ϊ�Ҷ�ͼ
	Mat gary;
	cvtColor(src, gary, COLOR_BGR2GRAY);
	// ��ȡ��Ϣ 
	int cols = gary.cols;// ��
	int rows = gary.rows;// ��
	int chnnales = gary.channels();// ͨ����1 ��ѹ�������ӵĴ�
	cout << cols << "," << rows << "," << chnnales;

	// Bitmap ����ת���� 4 ͨ�� �� һ��ͨ���Ϳ��Դ���Ҷ�
    // �Լ�ת��4ͨ��
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
		//��Ҫ�ж�ͨ��
			if (chnnales == 3){
				// ��ȡ���� at  Vec3b ������
				int b = gary.at<Vec3b>(i, j)[0];
				int g = gary.at<Vec3b>(i, j)[1];
				int r = gary.at<Vec3b>(i, j)[2];

				// �޸����� (��ƬЧ��)
				gary.at<Vec3b>(i, j)[0] = 255 - b;
				gary.at<Vec3b>(i, j)[1] = 255 - g;
				gary.at<Vec3b>(i, j)[2] = 255 - r;
			}else if (chnnales == 1){
				uchar pixels = gary.at<uchar>(i, j);
				gary.at<uchar>(i, j) = 255 - pixels;
			}
		}
	}
	


    //�ֶ�ת�Ҷ�ͼ
	// ��ȡ��Ϣ 
	int cols = src.cols;// ��
	int rows = src.rows;// ��
	int chnnales = src.channels();// 1 

    //���¶���gray����
	Mat gary(rows,cols,CV_8UC1);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (chnnales == 3){
				// ��ȡ���� at  Vec3b ������
				int b = src.at<Vec3b>(i, j)[0];
				int g = src.at<Vec3b>(i, j)[1];
				int r = src.at<Vec3b>(i, j)[2];

				// = ? 
				gary.at<uchar>(i, j) = 0.11f*r + 0.59*g+ 0.30*b;
			}
		}
	}

	// cvtColor(src, gary, COLOR_BGR2GRAY);

	// ��ʾ src ����ͼ��ʲôЧ����
	imwrite("C:/Users/hcDarren/Desktop/android/gary.png", gary);
	imshow("gary", gary);
	waitKey(0);
	return 0;
}


// ͼƬ��ˮӡ��ͼƬ���  ���Լ�����ָ��ȥʵ�֣�����
 //void addWeighted(InputArray src1, double alpha, InputArray src2, double beta, double gamma, OutputArray dst, int dtype=-1); ��һ��������InputArray���͵�src1����ʾ��Ҫ��Ȩ�ĵ�һ�����飬������һ��Mat��
 // ��һ��������src1��һ������
 //�ڶ���������alpha����ʾ��һ�������Ȩ��
 //������������src2����ʾ�ڶ������飬����Ҫ�͵�һ������ӵ����ͬ�ĳߴ��ͨ������
 //���ĸ�������beta����ʾ�ڶ��������Ȩ��ֵ��
 //�����������dst����������飬�����������������ӵ����ͬ�ĳߴ��ͨ������
 //������������gamma��һ���ӵ�Ȩ���ܺ��ϵı���ֵ��
 //���߸�������dtype��������еĿ�ѡ��ȣ���Ĭ��ֵ-1��;�������������������ͬ�����ʱ�������������Ϊ-1��Ĭ��ֵ��������ͬ��src1.depth����
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
	// ע������ͼƬ�Ĵ�С�����һ��
	Mat dst;
	// �ǳ���Ӳ��ֱ���������
	// add(src,logo,dst);
	//  alpha + beta ��ֵ���Դ���1
    //saturate_cast()  �������ת����ֵ��0-255
	// dst(x) = saturate_cast(src(x)*alpha + logo(x)*beta + gamma) 
	// addWeighted(src,0.8, logo, 0.2, 0.0, dst);

	// �ǵü�һ�� logo ��ȥ��ô�ӣ�˼·���뵽
    // ��ȡ����С��logoһ�£�Ȼ����������
	Mat srcROI = src(Rect(0, 0, logo.cols, logo.rows));
	// �����ʺ�ȥ��ˮӡ��ֻ�ʺ���ͼƬ���
	addWeighted(srcROI, 0.8, logo, 1, 0.0, srcROI);

	// ��ҵ��ɣ������ļ�ˮӡ

	imshow("dst", src);
	waitKey(0);
	return 0;
}

// 4. ���Ͷȡ����ȺͶԱȶ�
// ���Ͷȣ�ͼƬ���ӱ�����ʲô��˼�� ��ɫ�����죬�ʺ죬��÷��ϵȵ�   R -> 1.2R B -> 1.2B G -> 1.2G
// ���ȣ������ĳ̶�
// �Աȶ�: ͼ��Աȶ�ָ���� [1]  һ��ͼ�����������������İ׺���ĺ�֮�䲻ͬ���Ȳ㼶�Ĳ�������ָһ��ͼ��Ҷȷ���Ĵ�С
int main(){
	Mat src = imread("C:/Users/hcDarren/Desktop/android/test.png");
	if (src.empty()){
		cout << " src imread error " << endl;
		return -1;
	}

	// �˾� UI���ʦ �ٵ���һ�� ������һ�㣬�Աȶ��ٵ���һ�㣿
    // alpha ����ɱ���ȥ�� 1:500  10:5000  beta��
    //
	// ͨ�� alpha ���Ե��� ���Ͷ� , �Աȶ�
	// beta ����
	// F(R) = alpha*R + beta;
	// F(G) = alpha*G + beta;
	// F(B) = alpha*B + beta;
	// ��ȡ��Ϣ 
	int cols = src.cols;// ��
	int rows = src.rows;// ��
	int chnnales = src.channels();// 1 

	int alpha = 1;
	int beta = -50;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (chnnales == 3){
				// ��ȡ���� at  Vec3b ������
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
