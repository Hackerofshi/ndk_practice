package com.tech.pratice_ffmpeg.egl

import android.opengl.GLES20

object OpenGLTools {
    fun createTextureIds(count: Int): IntArray {
        val texture = IntArray(count)
        GLES20.glGenTextures(count, texture, 0)
        return texture
    }
}