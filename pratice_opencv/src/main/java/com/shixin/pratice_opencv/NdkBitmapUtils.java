package com.shixin.pratice_opencv;

import android.graphics.Bitmap;

public class NdkBitmapUtils {

    static {
        System.loadLibrary("native-lib");
    }

    public static native Bitmap againstWorld(Bitmap bitmap);
}
