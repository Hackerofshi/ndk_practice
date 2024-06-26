package com.shixin.pratice_opencv;

import android.graphics.Bitmap;

import org.opencv.core.MatOfPoint2f;
import org.opencv.core.Point;
import org.opencv.core.RotatedRect;
import org.opencv.imgproc.Imgproc;

public class NdkBitmapUtils {

    static {
        System.loadLibrary("native-lib");
    }

    /**
     * 实现逆世界效果
     *
     * @param bitmap 原图像
     * @return 逆世界
     */
    public static native Bitmap againstWorld(Bitmap bitmap);

    /**
     * 浮雕效果
     *
     * @param bitmap 原图像
     * @return 浮雕效果图像
     */
    public static final native Bitmap anaglyph(Bitmap bitmap);

    /**
     * 实现图片毛玻璃效果
     *
     * @param bitmap 原图像
     * @return 毛玻璃效果
     */
    public static final native Bitmap groundGlass(Bitmap bitmap);

    /**
     * 实现马赛克效果
     *
     * @param bitmap 原图像
     * @return 马赛克图片
     */
    public static final native Bitmap mosaic(Bitmap bitmap);

    /**
     * 实现图像油画效果
     *
     * @param bitmap
     * @return 油画效果图像
     */
    public static final native Bitmap oilPainting(Bitmap bitmap);

    /**
     * 灰度图像处理效果
     *
     * @param bitmap 原图像
     * @return 优化后的灰度图像
     */
    public static final native Bitmap garyOptimize(Bitmap bitmap);


    /**
     * 均值模糊
     *
     * @param bitmap 原图像
     * @return 模糊后的图像
     */
    public static native Bitmap blur(Bitmap bitmap);


    /**
     * 直方均衡
     *
     * @param bitmap 原图像
     * @return 均衡后的图像
     */
    public static native Bitmap hsv(Bitmap bitmap);


    public static native Bitmap filter2D(Bitmap bitmap);


    public static native Bitmap bilateralFilter(Bitmap bitmap);


    public static native Bitmap findContours(Bitmap bitmap);

    //判断是否为二维码。
    public static native Bitmap findQrCodeSimple(Bitmap bitmap);

    public static native Bitmap findQrCode(Bitmap bitmap);

    /**
     * 查找文档范围
     * @param nativeObj mat地址
     * @return bitmap
     */
    public static native Bitmap findDocument(long nativeObj);

    /**
     * 将mat转为bitmap
     * @param nativeObj mat地址
     * @return bitmap
     */
    public static native Bitmap matToBitmap(long nativeObj);

    /**
     * 去除文档上面的阴影
     * @param bitmap  图片的bitmap
     * @return bitmap
     */
    public static native Bitmap removeShadows(Bitmap bitmap);


    public static native void test();

}
