package com.shixin.ndk_practice.opengl;

public interface IFboRender {
    void onBindFbo();

    void onUnbindFbo();

    void onSurfaceCreated(int viewWidth, int viewHeight);

    void onSurfaceChanged(int width, int height);

    void onDrawFrame();
}
