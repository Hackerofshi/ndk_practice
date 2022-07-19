package com.shixin.ndk_practice.practicec;


import android.util.Log;

import java.util.UUID;

//测试访问java属性并且修改
public class Test1 {

    public String name = "test";
    public static int age = 1;

    public native void test1();

    public static native void test2();

    public int add(int i1, int i2) {
        int i = i1 + i2;
        Log.i("TAG", "add: " + i);
        return i;
    }

    public static String getUUID() {
        String s = UUID.randomUUID().toString();
        Log.i("UUID", "add: " + s);
        return s;
    }

    public native void callAddMethod();

    public static native void callUUIDMethod();


    public native void testConst();


    public native static Point createPoint();

    public native void nativeArr(int[] arr);

    static {
        System.loadLibrary("ndk_practice");
    }
}
