package com.shixin.bitmap_compress;

import android.graphics.Bitmap;


public class CompressUtils {

    static {
        System.loadLibrary("native-lib");
    }

    public static native int compressBitmap(Bitmap bitmap, int quality,
                                            String outFilPath,
                                            boolean optimize);

}
