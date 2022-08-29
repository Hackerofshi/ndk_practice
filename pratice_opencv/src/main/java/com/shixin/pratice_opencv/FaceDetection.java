package com.shixin.pratice_opencv;

import android.graphics.Bitmap;

public class FaceDetection {

    static {
        System.loadLibrary("native-lib");
    }
    /**
     * 检测人脸并保存人脸信息
     * @param mFaceBitmap
     */
    public native int faceDetectionSaveInfo(Bitmap mFaceBitmap);

    /**
     * 加载人脸识别的分类器文件
     * @param filePath
     */
    public native void loadCascade(String filePath);
}
