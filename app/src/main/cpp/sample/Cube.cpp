//
// Created by shixin on 2022/7/22.
//

#include "Cube.h"
#include "../glm/gtc/matrix_transform.hpp"

#include "../util/GLUtils.h"
#include "../util/LogUtil.h"

Cube::Cube() = default;

Cube::~Cube() {
    if (m_ProgramObj) {
        //  glDeleteProgram(m_ProgramObj);
        GLUtils::DeleteProgram(m_ProgramObj);
    }
}

void Cube::Init() {
    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec4 aColor;       \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out  vec4 vColor;                          \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = u_MVPMatrix * a_position; \n"
            "   vColor=aColor;                          \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "in vec4 vColor;                              \n"
            "out vec4 fragColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vColor;                       \n"
            "}                                            \n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj) {
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    }
}

void Cube::Draw(int screenW, int screenH) {
    //GLfloat COORDS_PER_VERTEX = 3;
    GLfloat cubePositions[] = {
            -1.0f, 1.0f, 1.0f,    //正面左上0
            -1.0f, -1.0f, 1.0f,   //正面左下1
            1.0f, -1.0f, 1.0f,    //正面右下2
            1.0f, 1.0f, 1.0f,     //正面右上3
            -1.0f, 1.0f, -1.0f,    //反面左上4
            -1.0f, -1.0f, -1.0f,   //反面左下5
            1.0f, -1.0f, -1.0f,    //反面右下6
            1.0f, 1.0f, -1.0f,     //反面右上7
    };
    GLushort index[] = {
            6, 7, 4, 6, 4, 5,    //后面
            0, 1, 5, 0, 5, 4,    //左面
            6, 5, 1, 6, 1, 2,    //下面
            6, 3, 7, 6, 2, 3,    //右面
            0, 3, 2, 0, 2, 1,    //正面
            0, 7, 3, 0, 4, 7,    //上面
    };

    GLfloat color[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f, 1.0f,
    };

    if (m_ProgramObj == 0) {
        return;
    }

    // Use the program object
    glUseProgram(m_ProgramObj);

    UpdateTransformMatrix(m_MVPMatrix, screenW, screenH);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);


    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, cubePositions);
    glEnableVertexAttribArray(0);

    //绘制三角形颜色
    glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, color);
    glEnableVertexAttribArray(1);

    //这里的count 不能用sizeOf()
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, index);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

}


void Cube::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {

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
    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 3.0f, 20.0f);
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(5, 5, 10), // Camera is at (5,5,1), in World Space
            // 相机位置
            glm::vec3(0, 0, 0), // and looks at the origin
            // 观测点位置
            glm::vec3(0, 1,
                      0)  // Head is up (set to 0,-1,0 to look upside-down)is up (set to 0,-1,0 to look upside-down)
            // up向量在xyz上的分量
    );

    // Model matrix : an identity matrix (model will be at the origin)
    // glm::mat4 Model = glm::mat4(1.0f);
    mvpMatrix = Projection * View;
}


void Cube::LoadImage(NativeImage *pImage) {

}


void Cube::Destroy() {

}
