//
// Created by 203 on 2021/8/31.
//

#include "TriangleSample.h"
#include "util/GLUtils.h"
#include "util/LogUtil.h"

TriangleSample::TriangleSample()
{

}

TriangleSample::~TriangleSample()
{
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
    }

}

void TriangleSample::Init()
{
    LOGCATE("TriangleSample::Draw");

    char vShaderStr[] =
            "#version 300 es                          \n"
            "layout(location = 0) in vec4 vPosition;  \n"
            "void main()                              \n"
            "{                                        \n"
            "   gl_Position = vPosition;              \n"
            "}                                        \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
            "}                                            \n";
    LOGCATE("TriangleSample::Draw");
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    LOGCATE("TriangleSample::Draw");

}

void TriangleSample::OnDraw()
{
    LOGCATE("TriangleSample::Draw");
    GLfloat vVertices[] = {
            0.0f,  0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
    };

    if(m_ProgramObj == 0)
        return;

    // Use the program object
    glUseProgram (m_ProgramObj);

    // Load the vertex data
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
    glEnableVertexAttribArray (0);

    glDrawArrays (GL_TRIANGLES, 0, 3);

}

void TriangleSample::LoadImage(NativeImage *pImage) {

}

void TriangleSample::Draw(int screenW, int screenH) {

}

void TriangleSample::Destroy() {

}