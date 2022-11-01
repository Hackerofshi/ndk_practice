//
// Created by shixin on 2022/10/31.
//

#ifndef NDK_PRACTICE_GL_PLAYER_H
#define NDK_PRACTICE_GL_PLAYER_H


#include "../source/v_decoder.h"
#include "../opengl/drawer/proxy/drawer_proxy.h"
#include "../opengl/drawer/video_drawer.h"
#include "../source/audio_decoder.h"
#include "../render/audio/audio_render.h"
#include "../render/video/opengl_render.h"
#include "../opengl/drawer/proxy/def_drawer_proxy_impl.h"
#include "../render/audio/opensl_render.h"


class GLPlayer {
private:
    VideoDecoder *m_v_decoder;
    OpenglRender *m_gl_render;

    DrawerProxy *m_v_drawer_proxy;
    VideoDrawer *m_v_drawer;

    AudioDecoder *m_a_decoder;
    AudioRender *m_a_render;

public:
    GLPlayer(JNIEnv *jniEnv, jstring path);
    ~GLPlayer();

    void SetSurface(jobject surface);
    void PlayOrPause();
    void Release();
};


#endif //NDK_PRACTICE_GL_PLAYER_H
