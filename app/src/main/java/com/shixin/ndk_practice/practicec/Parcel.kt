package com.shixin.ndk_practice.practicec

import android.os.Parcel


public class Parcel {
    private var mNativePtr: Long = 0

    init {
        System.loadLibrary("ndk_practice")

        mNativePtr = nativeCreate()
    }

    //c层 构建一个parcel.cpp对象
    private external fun nativeCreate(): Long

    fun writeInt(`val`: Int) {
        nativeWriteInt(mNativePtr, `val`)
    }

    fun setDataPosition(pos: Int) {
       nativeSetDataPosition(mNativePtr, pos)
    }
    fun readInt(): Int {
        return nativeReadInt(mNativePtr)
    }

    //读 int
    private external fun nativeReadInt(mNativePtr: Long): Int

    //写 int 数据
    private external fun nativeWriteInt(mNativePtr: Long, i: Int)

    //写完以后重新设置位置
    private external fun nativeSetDataPosition(mNativePtr: Long, pos: Int)

}