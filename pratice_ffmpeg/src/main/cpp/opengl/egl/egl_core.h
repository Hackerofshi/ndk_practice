//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_EGL_CORE_H
#define NDK_PRACTICE_EGL_CORE_H
#include "../../utils/log.h"


extern "C" {
#include "EGL/egl.h"
#include "EGL/eglext.h"
}


class EglCore {
private:
    const char *TAG = "EglCore";

    //EGL显示窗口
    EGLDisplay m_egl_dsp = EGL_NO_DISPLAY;

    //EGL上下文
    EGLContext m_egl_cxt = EGL_NO_CONTEXT;
    //EGL配置
    EGLConfig m_egl_config;

    EGLConfig GetEGLConfig();

public:
    EglCore();

    ~EglCore();

    bool Init(EGLContext share_ctx);

    /**
     * 根据本地窗口创建显示页面
     *
     */
    EGLSurface createWindSurface(ANativeWindow *window);

    /**
     * 创建离屏渲染表面
     * @param width 表面宽
     * @param height 表面高
     * @return
     */
     EGLSurface CreateOffScreenSurface(int width,int height);

    /**
    * 将OpenGL上下文和线程进行绑定
    * @param egl_surface
    */
    void MakeCurrent(EGLSurface egl_surface);

    /**
     * 将缓存数据交换到前台进行显示
     * @param egl_surface
     */
     void SwapBuffers(EGLSurface egl_surface);


    /**
     * 释放显示
     * @param elg_surface
     */
     void DestroySurface(EGLSurface egl_surface);

     //释放
     void release();

};


#endif //NDK_PRACTICE_EGL_CORE_H
