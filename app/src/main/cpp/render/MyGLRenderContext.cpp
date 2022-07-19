//
// Created by 203 on 2021/8/31.
//

#include "MyGLRenderContext.h"
#include "../util/LogUtil.h"
#include "../util/ImageDef.h"
#include "../sample/TriangleSample.h"


MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {
    m_Sample = new TriangleSample();
};


MyGLRenderContext::~MyGLRenderContext() = default;;

void MyGLRenderContext::SetImageData(int format, int width, int height, uint8_t *pData) {
   // LOGCATE("MyGLRenderContext::SetImageData format=%d, width=%d, height=%d, pData=%p", format,
    //        width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;

    }
}

void MyGLRenderContext::OnSurfaceCreated() {
    LOGCATE("MyGLRenderContext::OnSurfaceCreated");
    glClearColor(1.0f,1.0f,0.5f,1.0f);
    LOGCATE("MyGLRenderContext::OnSurfaceCreated1");
    m_Sample->Init();
    LOGCATE("MyGLRenderContext::OnSurfaceCreated");
}


void MyGLRenderContext::OnSurfaceChanged(int width, int height) {
   // LOGCATE("MyGLRenderContext::OnSurfaceChanged [w, h] = [%d, %d]", width, height);
    glViewport(0,0,width,height);
}

void MyGLRenderContext::OnDrawFrame() {
   // LOGCATE("MyGLRenderContext::OnDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_Sample->OnDraw();
}

MyGLRenderContext *MyGLRenderContext::GetInstance(){
    if(m_pContext == nullptr){
        m_pContext = new MyGLRenderContext();
    }
    return m_pContext;
}

void MyGLRenderContext::DestroyInstance() {
    if(m_pContext){
        delete m_pContext;
        m_pContext = nullptr;
    }
}
