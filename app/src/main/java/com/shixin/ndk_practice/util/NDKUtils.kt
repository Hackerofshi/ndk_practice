package com.shixin.ndk_practice.util

object NDKUtils {

    init {
        System.loadLibrary("ndk_practice")
    }

    external fun read(path: String)

    external fun write(path: String)

    external fun testThread()
}