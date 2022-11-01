//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_EGL_SURFACE_H
#define NDK_PRACTICE_EGL_SURFACE_H

#include "egl_core.h"

class EglSurface {
private:
    const char *TAG = "EglSurface";

    ANativeWindow *m_native_window = nullptr;

    EglCore *m_core;

    EGLSurface m_surface;
public:
    EglSurface();

    ~EglSurface();

    bool Init();

    void CreateEglSurface(ANativeWindow *native_window, int width, int height);

    void MakeCurrent();

    void SwapBuffers();

    void DestroyEglSurface();

    void Release();

};


#endif //NDK_PRACTICE_EGL_SURFACE_H
