//
// Created by shixin on 2022/10/11.
//

#include <jni.h>
#include <string>
#include <unistd.h>
#include "media/player.h"
#include "player/gl_player.h"


extern "C" {
#include "libavcodec/avcodec.h"
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavcodec/jni.h>

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    av_jni_set_java_vm(vm, reserved);
    return JNI_VERSION_1_4;
}

JNIEXPORT jstring JNICALL
Java_com_tech_pratice_1ffmpeg_Player_ffmpegInfo(JNIEnv *env, jobject thiz) {
    char info[40000] = {0};
    AVCodec *c_temp = av_codec_next(nullptr);
    while (c_temp != nullptr) {
        if (c_temp->decode != nullptr) {
            sprintf(info, "%sdecode:", info);
        } else {
            sprintf(info, "%sencode:", info);
        }
        switch (c_temp->type) {
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s(video):", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s(audio):", info);
                break;
            default:
                sprintf(info, "%s(other):", info);
                break;
        }
        sprintf(info, "%s[%s]\n", info, c_temp->name);
        c_temp = c_temp->next;
    }


    return env->NewStringUTF(info);
}

JNIEXPORT jint JNICALL
Java_com_tech_pratice_1ffmpeg_Player_createPlayer(JNIEnv *env, jobject thiz, jstring path,
                                                  jobject surface) {
    auto *player = new Player(env, path, surface);
    return (jint) player;
}
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_play(JNIEnv *env, jobject thiz, jint player) {
    auto *p = (Player *) player;
    p->play();
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_pause(JNIEnv *env, jobject thiz, jint player) {

}
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_stop(JNIEnv *env, jobject thiz, jint player) {
    auto *p = (Player *) player;
    p->Release();
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_createGLPlayer(JNIEnv *env, jobject thiz,
                                                                    jstring path, jobject surface) {
    GLPlayer *player = new GLPlayer(env, path);
    player->SetSurface(surface);
    return (jint) player;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_playOrPause(JNIEnv *env, jobject thiz,
                                                                 jint player) {
    GLPlayer *p = (GLPlayer *) player;
    p->PlayOrPause();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_stop(JNIEnv *env, jobject thiz,
                                                          jint player) {
    GLPlayer *p = (GLPlayer *) player;
    p->Release();
}


}


