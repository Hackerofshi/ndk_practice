//
// Created by shixin on 2022/10/31.
//

#include "opengl_render.h"


OpenglRender::OpenglRender(JNIEnv *env, DrawerProxy *drawerProxy) : m_drawer_proxy(drawerProxy),
                                                                    m_egl_surface(new EglSurface) {
    this->m_env = env;

    //获取jvm虚拟机，为创建线程作准备
    env->GetJavaVM(&m_jvm_for_thread);
    InitRenderThread();
}

OpenglRender::~OpenglRender() {
    delete m_egl_surface;
}


void OpenglRender::sRenderThread(const std::shared_ptr<OpenglRender> &that) {
    JNIEnv *env;

    //将线程附加到虚拟机，并获取env
    if (that->m_jvm_for_thread->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE(that->TAG, "线程初始化异常");
        return;
    }

    if (!that->InitEGL()) {
        //解除线程jvm关联
        that->m_jvm_for_thread->DetachCurrentThread();
        return;
    }
    while (true) {
        switch (that->m_state) {
            case FRESH_SURFACE:
                LOGI("Openg GL FRESH_SURFACE %s", "Loop Render FRESH_SURFACE")
                that->InitDspWindow(env);
                that->CreateSurface();
                that->m_state = RENDERING;
                break;
            case RENDERING:
                LOGI("Openg GL RENDERING %s", "Loop Render RENDERING")
                that->Render();
                break;
            case SURFACE_DESTROY:
                LOGI("Openg GL SURFACE_DESTROY %s", "Loop Render SURFACE_DESTROY")
                that->DestroySurface();
                that->m_state = NO_SURFACE;
                break;
            case STOP:
                LOGI("Openg GL STOP %s", "Loop Render STOP")
                //解除线程和jvm关联
                that->ReleaseRender();
                that->m_jvm_for_thread->DetachCurrentThread();
                return; //结束循环退出
            case NO_SURFACE:
            default:
                break;
        }
        usleep(20000);
    }
}


void OpenglRender::SetSurface(jobject surface) {
    if (nullptr != surface) {
        m_surface_ref = m_env->NewGlobalRef(surface);
        m_state = FRESH_SURFACE;
    } else {
        m_env->DeleteGlobalRef(m_surface_ref);
        m_state = SURFACE_DESTROY;
    }
}


void OpenglRender::SetOffScreenSize(int width, int height) {
    m_window_width = width;
    m_window_height = height;
    m_state = FRESH_SURFACE;
}

void OpenglRender::RequestRgbaData() {
    m_need_output_pixels = true;
}

void OpenglRender::Stop() {
    m_state = STOP;
}


void OpenglRender::InitRenderThread() {
    // 使用智能指针，线程结束时，自动删除本类指针
    std::shared_ptr<OpenglRender> that(this);
    std::thread t(sRenderThread, that);
    t.detach();
}

bool OpenglRender::InitEGL() {
    m_egl_surface = new EglSurface();
    return m_egl_surface->Init();
}

void OpenglRender::InitDspWindow(JNIEnv *env) {
    if (m_surface_ref != nullptr) {
        m_native_window = ANativeWindow_fromSurface(env, m_surface_ref);
        //绘制区域的宽高
        m_window_width = ANativeWindow_getWidth(m_native_window);
        m_window_height = ANativeWindow_getHeight(m_native_window);

        //设置宽高限制缓冲区中的像素数量
        ANativeWindow_setBuffersGeometry(m_native_window, m_window_width, m_window_height,
                                         WINDOW_FORMAT_RGBA_8888);
        LOGI(TAG, "View Port width: %d, height: %d", m_window_width, m_window_height)
    }
}

void OpenglRender::CreateSurface() {
    m_egl_surface->CreateEglSurface(m_native_window, m_window_width, m_window_height);
    glViewport(0, 0, m_window_width, m_window_height);
}

void OpenglRender::DestroySurface() {
    m_egl_surface->DestroyEglSurface();
    ReleaseWindow();
}

void OpenglRender::Render() {
    //LOGI(TAG, "Loop Render OpenGLRender::Render()")
    if (m_state == RENDERING) {
        m_drawer_proxy->Draw();
        m_egl_surface->SwapBuffers();
       // LOGI(TAG, "渲染画面")
        if (m_need_output_pixels && m_pixel_receiver != nullptr) {
            m_need_output_pixels = false;
            Render();

            size_t size = m_window_width * m_window_height * 4 * sizeof(uint8_t);

            uint8_t *rgb = (uint8_t *) malloc(size);
            if (rgb == nullptr) {
                realloc(rgb, size);
                LOGE(TAG, "内存分配失败： %d", rgb)
            }
            glReadPixels(0, 0, m_window_width, m_window_height, GL_RGBA, GL_UNSIGNED_BYTE, rgb);
            m_pixel_receiver->ReceivePixel(rgb);
        }

    }
}

void OpenglRender::ReleaseRender() {
    ReleaseDrawers();
    ReleaseSurface();
    ReleaseWindow();
}

void OpenglRender::ReleaseDrawers() {
    if (m_drawer_proxy != nullptr) {
        m_drawer_proxy->Release();
        delete m_drawer_proxy;
        m_drawer_proxy = nullptr;
    }
}

void OpenglRender::ReleaseSurface() {
    if (m_egl_surface != nullptr) {
        m_egl_surface->Release();
        delete m_egl_surface;
        m_egl_surface = nullptr;
    }
}

void OpenglRender::ReleaseWindow() {
    if (m_native_window != nullptr) {
        ANativeWindow_release(m_native_window);
        m_native_window = nullptr;
    }
}