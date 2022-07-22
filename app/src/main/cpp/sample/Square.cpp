//
// Created by shixin on 2022/7/21.
//

#include "Square.h"
#include "../glm/gtc/matrix_transform.hpp"

#include "../util/GLUtils.h"
#include "../util/LogUtil.h"

Square::Square() {

}

Square::~Square() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}

void Square::Init() {
    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = u_MVPMatrix * a_position; \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
            "}                                            \n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj) {
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    }
}

void Square::Draw(int screenW, int screenH) {
    GLfloat vVertices[] = {
            -0.5f, 0.5f, 0.0f, // top left
            -0.5f, -0.5f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f, // bottom right
            0.5f, 0.5f, 0.0f  // top right
    };

    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    if (m_ProgramObj == 0) {
        return;
    }

    // Use the program object
    glUseProgram(m_ProgramObj);

    UpdateTransformMatrix(m_MVPMatrix, screenW, screenH);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);


    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

}

void Square::LoadImage(NativeImage *pImage) {

}


void Square::Destroy() {

}


void Square::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {

    float ratio = (float) screenW / screenH;
    //Matrix.frustumM (float[] m,         //接收透视投影的变换矩阵
    //                int mOffset,        //变换矩阵的起始位置（偏移量）
    //                float left,         //相对观察点近面的左边距
    //                float right,        //相对观察点近面的右边距
    //                float bottom,       //相对观察点近面的下边距
    //                float top,          //相对观察点近面的上边距
    //                float near,         //相对观察点近面距离
    //                float far)          //相对观察点远面距离
    //————————————————


    //Matrix.orthoM (float[] m,           //接收正交投影的变换矩阵
    //                int mOffset,        //变换矩阵的起始位置（偏移量）
    //                float left,         //相对观察点近面的左边距
    //                float right,        //相对观察点近面的右边距
    //                float bottom,       //相对观察点近面的下边距
    //                float top,          //相对观察点近面的上边距
    //                float near,         //相对观察点近面距离
    //                float far)          //相对观察点远面距离
    //————————————————
    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 3.0f, 7.0f);
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            // 相机位置
            glm::vec3(0, 0, 0), // and looks at the origin
            // 观测点位置

            glm::vec3(0, 1,0)  // Head is up (set to 0,-1,0 to look upside-down)is up (set to 0,-1,0 to look upside-down)
            // up向量在xyz上的分量
    );

    // Model matrix : an identity matrix (model will be at the origin)
    // glm::mat4 Model = glm::mat4(1.0f);
    mvpMatrix = Projection * View;
}