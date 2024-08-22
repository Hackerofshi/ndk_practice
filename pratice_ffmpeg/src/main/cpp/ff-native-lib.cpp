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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <android/log.h>
#include "./source/test/avio_test.h"

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

JNIEXPORT jint JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_createGLPlayer(JNIEnv *env, jobject thiz,
                                                                    jstring path, jobject surface) {
    GLPlayer *player = new GLPlayer(env, path);
    player->SetSurface(surface);
    return (jint) player;
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_playOrPause(JNIEnv *env, jobject thiz,
                                                                 jint player) {
    GLPlayer *p = (GLPlayer *) player;
    p->PlayOrPause();
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_FFmpegGLPlayerActivity_stop(JNIEnv *env, jobject thiz,
                                                          jint player) {
    GLPlayer *p = (GLPlayer *) player;
    p->Release();
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_testAVIOContext(JNIEnv *env, jobject thiz, jstring str) {
    const char *cStr = env->GetStringUTFChars(str, NULL);
    testPath(cStr);
    env->ReleaseStringUTFChars(str, cStr);
    LOG_INFO("123123", "", "fff");
}


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_startTCPClient(JNIEnv *env, jobject thiz, jstring ip,
                                                    jint port) {

    const char *server_ip = env->GetStringUTFChars(ip, NULL);
    int server_port = port;

    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[1024];

    // 1. 创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("Socket creation error");
        return;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    // 2. 将 IP 地址转换为二进制形式
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        LOGE("Invalid address/ Address not supported");
        close(sockfd);
        return;
    }

    // 3. 连接到服务器
    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        LOGE("Connection Failed");
        close(sockfd);
        return;
    }

    LOGI("Connected to the server");

    // 4. 从服务器接收数据
    while (1) {
        ssize_t bytes_read = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes_read < 0) {
            LOGE("Read error");
            break;
        } else if (bytes_read == 0) {
            LOGI("Server closed the connection");
            break;
        }
        buffer[bytes_read] = '\0'; // 确保字符串以 NULL 结尾
        LOGI("Received: %s", buffer);
    }

    // 5. 关闭套接字
    close(sockfd);
    env->ReleaseStringUTFChars(ip, server_ip);
}

#define PORT 1234
#define BUFFER_SIZE 1024


JNIEXPORT void JNICALL
Java_com_tech_pratice_1ffmpeg_Player_startUDPClient(JNIEnv *env, jobject thiz) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in servaddr;

    // 创建UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return;
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // 设置服务器地址
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定socket
    if (bind(sockfd, (const struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        close(sockfd);
        return;
    }

    while (1) {
        int n;
        socklen_t len = sizeof(servaddr);
        n = recvfrom(sockfd, (char *) buffer, BUFFER_SIZE, MSG_WAITALL,
                     (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
        LOGI("Received: %s\n", buffer);
    }
    close(sockfd);
}

}



