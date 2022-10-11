//
// Created by shixin on 2022/10/11.
//

#include "OpenSLAudioPlay.h"
#include "OpenSLRecord.h"

/**
 * 播放 pcmFile
 */
FILE *pcmFile = 0;

OpenSLAudioPlay *slAudioPlayer = nullptr;
OpenSLRecord *record = nullptr;

/**
 * 是否正在播放
 */
bool isPlaying = false;

void *playThreadFunc(void *arg);

void *playThreadFunc(void *arg) {
    const int bufferSize = 2048;
    short buffer[bufferSize];
    while (isPlaying && !feof(pcmFile)) {
        fread(buffer, 1, bufferSize, pcmFile);
        slAudioPlayer->enqueueSample(buffer, bufferSize);
    }
    return 0;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_tech_pratice_1audio_AudioPlayer_nativePlayPcm(JNIEnv *env, jclass clazz,
                                                       jstring pcm_path) {
    //将 Java 传递过来的 String 转为 C 中的 char *
    const char *_pcmPath = env->GetStringUTFChars(pcm_path, NULL);

    //如果已经实例化了，就释放资源
    if (slAudioPlayer) {
        slAudioPlayer->release();
        delete slAudioPlayer;
        slAudioPlayer = nullptr;
    }

    //实例化 openslAudioPlay
    slAudioPlayer = new OpenSLAudioPlay(44100, SAMPLE_FORMAT_16, 1);
    slAudioPlayer->init();
    pcmFile = fopen(_pcmPath, "r");
    isPlaying = true;
    pthread_t playThread;

    pthread_create(&playThread, nullptr, playThreadFunc, 0);
    env->ReleaseStringUTFChars(pcm_path, _pcmPath);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_tech_pratice_1audio_AudioPlayer_nativeStopPcm(JNIEnv *env, jclass clazz) {
    isPlaying = false;
    if (slAudioPlayer) {
        slAudioPlayer->release();
        delete slAudioPlayer;
        slAudioPlayer = nullptr;
    }

    if (pcmFile) {
        fclose(pcmFile);
        pcmFile = nullptr;
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_tech_pratice_1audio_AudioRecord_startRecord(JNIEnv *env, jclass clazz, jstring pcm_path) {
    //将 Java 传递过来的 String 转为 C 中的 char *
    const char *_pcmPath = env->GetStringUTFChars(pcm_path, NULL);

    //如果已经实例化了，就释放资源
    if (record) {
        record->release();
        delete record;
        record = nullptr;
    }

    pcmFile = fopen(_pcmPath, "w");

    record = new OpenSLRecord(pcmFile);
    record->init();
    if (record) {
        record->startRecord();
    }

    env->ReleaseStringUTFChars(pcm_path, _pcmPath);
}