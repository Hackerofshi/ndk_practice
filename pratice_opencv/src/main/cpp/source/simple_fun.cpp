//
// Created by shixin on 2022/10/4.
//

//int main(){
//    /// 创建一个图形
//    const int r = 100;
//    Mat src = Mat::zeros(Size(4 * r, 4 * r), CV_8UC1);
//
//    /// 绘制一系列点创建一个轮廓:
//    vector<Point2f> vert(6);
//
//    vert[0] = Point(1.5*r, 1.34*r);
//    vert[1] = Point(1 * r, 2 * r);
//    vert[2] = Point(1.5*r, 2.866*r);
//    vert[3] = Point(2.5*r, 2.866*r);
//    vert[4] = Point(3 * r, 2 * r);
//    vert[5] = Point(2.5*r, 1.34*r);
//
//    /// 在src内部绘制
//    for (int j = 0; j < 6; j++)
//    {
//        line(src, vert[j], vert[(j + 1) % 6], Scalar(255), 3, 8);
//    }
//
//    // 查找轮廓
//    vector < vector<Point>> contours;
//    findContours(src, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//
//    // 计算到轮廓的距离
//    Mat raw_dist(src.size(), CV_32FC1);
//    for (int rows = 0; rows < src.rows; rows++)
//    {
//        for (int cols = 0; cols < src.cols; cols++)
//        {
//            raw_dist.at<float>(rows, cols) = pointPolygonTest(contours[0], Point2f(cols, rows), true);
//        }
//    }
//
//    // 优化
//    Mat drawing = Mat::zeros(src.size(), CV_8UC3);
//    for (int rows = 0; rows < src.rows; rows++)
//    {
//        for (int cols = 0; cols < src.cols; cols++)
//        {
//            // raw_dist.at<float>(rows, cols) = pointPolygonTest(contours[0], Point2f(rows, cols), true);
//            if (raw_dist.at<float>(rows, cols) < 0 ){// 外面
//                drawing.at<Vec3b>(rows, cols)[0] = saturate_cast<int>((int)abs(raw_dist.at<float>(rows, cols)));
//            }
//            else if (raw_dist.at<float>(rows, cols) > 0){ // 里面
//                drawing.at<Vec3b>(rows, cols)[2] = saturate_cast<int>((int)(raw_dist.at<float>(rows, cols)));
//            }
//            else{// 矩形上面
//                drawing.at<Vec3b>(rows, cols)[0] = 255;
//                drawing.at<Vec3b>(rows, cols)[1] = 255;
//                drawing.at<Vec3b>(rows, cols)[2] = 255;
//            }
//        }
//    }
//
//    imshow("drawing", drawing);
//
//
//    imshow("src", src);
//    waitKey(0);
//    getchar();
//}