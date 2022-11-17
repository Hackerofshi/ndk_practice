//
// Created by shixin on 2022/11/11.
//

#include "BasicLighting.h"


BasicLighting::BasicLighting() {

}

BasicLighting::~BasicLighting() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}

void BasicLighting::LoadImage(NativeImage *pImage) {

}

void BasicLighting::Init() {
    char vShaderStr[] =
            "layout (location = 0) in vec3 aPos;    \n"
            "uniform mat4 model;                    \n"
            "uniform mat4 view;                     \n"
            "uniform mat4 projection;               \n"
            " void main()                           \n"
            " {                                     \n"
            " gl_Position = projection * view * model * vec4(aPos, 1.0);  \n"
            " }                                     \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "out vec4 FragColor;                         \n"
            "uniform vec3 objectColor;  \n"
            "uniform vec3 lightColor;  \n"
            "void main()  \n"
            " {                                     \n"
            "       FragColor = vec4(lightColor * objectColor, 1.0);  \n"
            " }                                     \n";
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader,
                                          m_FragmentShader);
}

void BasicLighting::Draw(int screenW, int screenH) {

}

void BasicLighting::Destroy() {

}

void BasicLighting::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {
    GLSampleBase::UpdateTransformMatrix(mvpMatrix, screenW, screenH);
}
