package com.shixin.ndk_practice.record;

import android.content.Context;

import javax.microedition.khronos.egl.EGLContext;

/**
 * Created by hcDarren on 2019/7/13.
 * 默认彩色视频的录制
 */
public class DefaultVideoRecorder extends BaseVideoRecorder {
    public DefaultVideoRecorder(Context context, EGLContext eglContext, int textureId) {
        super(context, eglContext);
        setRenderer(new RecorderRenderer(context, textureId));
    }
}
