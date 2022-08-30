package com.shixin.pratice_opencv;

import android.graphics.Bitmap;

/**
 * Created by hcDarren on 2018/7/28.
 */

public class BankCardOcr {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    public static native String cardOcr(Bitmap bitmap);
}
