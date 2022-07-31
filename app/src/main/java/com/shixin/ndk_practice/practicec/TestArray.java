package com.shixin.ndk_practice.practicec;

public class TestArray {
    public static String name = "";
    public static String name1 = "";
    public static String name2 = "";

    static {
        System.loadLibrary("ndk_practice");
    }

    public static native void sort(int[] arr);


    //局部应用和全局引用
    public static native void localRef();

    public static native void globalRef(String str);

    public static native String getGlobalRef();

    public static native String deleteGlobalRef();

    //缓存策略，native层有一大推方法要去获取name属性
    public static native void staticLocalCache(String name);

    public static native void initStaticCache();

    public static native void exeception() throws NoSuchFieldException;


    public static native void arraycopy(Object src,  int  srcPos,
                                        Object dest, int destPos,
                                        int length);

}
