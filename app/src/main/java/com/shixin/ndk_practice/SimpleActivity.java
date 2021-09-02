package com.shixin.ndk_practice;

import android.opengl.GLSurfaceView;
import com.shixin.ndk_practice.opengl.NativeSimpleRenderer;

/**
 * @anchor: andy
 * @date: 2018-11-02
 * @description:
 */
public class SimpleActivity extends AbsGLSurfaceActivity {

    @Override
    protected GLSurfaceView.Renderer bindRenderer() {
        return new NativeSimpleRenderer();
    }
}
