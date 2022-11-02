//
// Created by shixin on 2022/10/31.
//

#include "video_drawer.h"

#define GET_STR(x) #x

VideoDrawer::VideoDrawer() : Drawer(0, 0) {

}

VideoDrawer::~VideoDrawer() {

}

void VideoDrawer::InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) {
    SetSize(video_width, video_height);
    dst_size[0] = video_width;
    dst_size[1] = video_height;
}

void VideoDrawer::Render(OneFrame *on_frame) {
    cst_data = on_frame->data;
}

void VideoDrawer::ReleaseRender() {

}

const char *VideoDrawer::GetVertexShader() {
    const char *vertexShader = GET_STR(
            attribute vec4 aPosition;
            attribute vec2 aCoordinate;
            varying vec2 vCoordinate;
            uniform mat4 u_MVPMatrix;
            void main() {
                gl_Position = u_MVPMatrix * aPosition;
                vCoordinate = aCoordinate;
            }
    );
    return vertexShader;
}

const char *VideoDrawer::GetFragmentShader() {
    const char *shader1 = GET_STR(
            precision mediump float;
            uniform sampler2D uTexture;
            varying vec2 vCoordinate;
            void main() {
                vec4 color = texture2D(uTexture, vCoordinate);
                float gray = (color.r + color.g + color.b) / 3.0;
                gl_FragColor = vec4(gray, gray, gray, 1.0);
            }
    );
    return shader1;
}

void VideoDrawer::InitCstShaderHandler() {

}

void VideoDrawer::BindTexture() {
    ActivateTexture();
}

void VideoDrawer::PrepareDraw() {
    if (cst_data != nullptr) {
        glTexImage2D(GL_TEXTURE_2D, 0, // level一般为0
                     GL_RGBA, //纹理内部格式
                     origin_width(), origin_height(), // 画面宽高
                     0, // 必须为0
                     GL_RGBA, // 数据格式，必须和上面的纹理格式保持一直
                     GL_UNSIGNED_BYTE, // RGBA每位数据的字节数，这里是BYTE: 1 byte
                     cst_data);// 画面数据
    }
}

void VideoDrawer::DoneDraw() {

}


