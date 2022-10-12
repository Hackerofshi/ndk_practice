//
// Created by shixin on 2022/10/12.
//

#ifndef NDK_PRACTICE_PLAYER_H
#define NDK_PRACTICE_PLAYER_H

#include "../source/v_decoder.h"
#include "../render/video/native_render.h"

class Player {
private:
    VideoDecoder *m_v_decoder;
    VideoRender *m_v_render;

public:
    Player(JNIEnv *jniEnv, jstring path, jobject surface);

    ~Player();

    void play();

    void pause();
};


#endif //NDK_PRACTICE_PLAYER_H
