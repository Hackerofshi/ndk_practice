package com.shixin.ndk_practice.practicec;

import android.graphics.Bitmap;

public class Compressor {
    static {
        System.loadLibrary("ndk_practice");
    }

    public native  static void native_Compress(Bitmap bitmap, int q, String path);

    public native void convertBmp(Bitmap bitmapIn, Bitmap bitmapOut);

}
