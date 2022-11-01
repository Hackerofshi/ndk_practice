//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_OPENGL_RENDER_H
#define NDK_PRACTICE_OPENGL_RENDER_H


#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "../../opengl/egl/egl_surface.h"
#include "../../opengl/drawer/proxy/drawer_proxy.h"
#include "opengl_pixel_receiver.h"
#include <memory>
#include <thread>
#include <unistd.h>
#include <libavutil/mem.h>


class OpenglRender {
private:
    const char *TAG = "OpenGLRender";

    enum STATE {
        NO_SURFACE, //没有有效的surface
        FRESH_SURFACE,//持有一个未初始化的新的surface
        RENDERING,//初始化完毕，可以开始渲染
        SURFACE_DESTROY,//surface销毁
        STOP //停止绘制
    };
    JNIEnv *m_env = nullptr;

    //线程依附的jvm环境
    JavaVM *m_jvm_for_thread = nullptr;

    // Surface引用，必须使用引用，否则无法在线程中操作
    jobject m_surface_ref = nullptr;

    //本地屏幕
    ANativeWindow *m_native_window = nullptr;

    //EGL显示surface
    EglSurface *m_egl_surface = nullptr;

    // 绘制代理器
    DrawerProxy *m_drawer_proxy = nullptr;

    int m_window_width = 0;

    int m_window_height = 0;

    bool m_need_output_pixels = false;

    OpenGLPixelReceiver *m_pixel_receiver = NULL;

    STATE m_state = NO_SURFACE;

    //初始化相关方法
    void InitRenderThread();

    bool InitEGL();

    void InitDspWindow(JNIEnv *env);

    //创建surface
    void CreateSurface();

    void DestroySurface();

    //渲染方法
    void Render();

    //释放资源相关方法
    void ReleaseRender();

    void ReleaseDrawers();

    void ReleaseSurface();

    void ReleaseWindow();

    // 渲染线程回调方法
    static void sRenderThread(const std::shared_ptr<OpenglRender> &that);

public:
    OpenglRender(JNIEnv *env, DrawerProxy *drawerProxy);

    ~OpenglRender();

    void SetPixelReceiver(OpenGLPixelReceiver *receiver) {
        m_pixel_receiver = receiver;
    }

    void SetSurface(jobject surface);

    void SetOffScreenSize(int width, int height);

    void RequestRgbaData();

    void Stop();
};


#endif //NDK_PRACTICE_OPENGL_RENDER_H
