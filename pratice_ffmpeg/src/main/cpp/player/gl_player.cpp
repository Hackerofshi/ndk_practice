//
// Created by shixin on 2022/10/31.
//

#include "gl_player.h"


GLPlayer::GLPlayer(JNIEnv *jniEnv, jstring path) {
    //解码线程
    m_v_decoder = new VideoDecoder(jniEnv,path);

    //OpenGl 渲染
    m_v_drawer = new VideoDrawer();
    m_v_decoder->setRender(m_v_drawer);


    auto *proxyImpl = new DefDrawerProxyImpl();
    proxyImpl->AddDrawer(m_v_drawer);


    //opengl 线程
    m_v_drawer_proxy = proxyImpl;
    m_gl_render = new OpenglRender(jniEnv, m_v_drawer_proxy);

    // 音频解码
    m_a_decoder = new AudioDecoder(jniEnv, path, false);
    m_a_render = new OpenSLRender();
    m_a_decoder->SetRender(m_a_render);

}

GLPlayer::~GLPlayer() {
    // 此处不需要 delete 成员指针
    // 在BaseDecoder 和 OpenGLRender 中的线程已经使用智能指针，会自动释放相关指针
}

void GLPlayer::SetSurface(jobject surface) {
    m_gl_render->SetSurface(surface);
}

void GLPlayer::PlayOrPause() {
    if (!m_v_decoder->IsRunning()) {
        LOGI("Player %s", "播放视频")
        m_v_decoder->GoOn();
    } else {
        LOGI("Player %s", "暂停视频")
        m_v_decoder->Pause();
    }
    if (!m_a_decoder->IsRunning()) {
        LOGI("Player %s", "播放音频")
        m_a_decoder->GoOn();
    } else {
        LOGI("Player %s", "暂停音频")
        m_a_decoder->Pause();
    }
}

void GLPlayer::Release() {
    m_gl_render->Stop();
    m_v_decoder->Stop();
    m_a_decoder->Stop();
}
