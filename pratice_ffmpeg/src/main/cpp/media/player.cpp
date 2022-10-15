//
// Created by shixin on 2022/10/12.
//

#include "player.h"

Player::Player(JNIEnv *jniEnv, jstring path, jobject surface) {
    m_v_decoder = new VideoDecoder(jniEnv, path);
    m_v_render = new NativeRender(jniEnv, surface);
    m_v_decoder->setRender(m_v_render);


    //音频解码
    audioDecoder = new AudioDecoder(jniEnv, path, false);
    audioRender = new OpenSLRender();
    audioDecoder->SetRender(audioRender);


}

Player::~Player() {
    // 此处不需要 delete 成员指针
    // 在BaseDecoder中的线程已经使用智能指针，会自动释放
}

void Player::play() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->GoOn();
        audioDecoder->GoOn();
    }
}

void Player::pause() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->Pause();
        audioDecoder->Pause();
    }
}


void Player::Release() {
    m_v_decoder->Stop();
    audioDecoder->Stop();
}
