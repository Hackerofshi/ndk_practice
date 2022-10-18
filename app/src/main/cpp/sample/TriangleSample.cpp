//
// Created by 203 on 2021/8/31.
//

#include "../glm/gtc/matrix_transform.hpp"

#include "TriangleSample.h"
#include "../util/GLUtils.h"
#include "../util/LogUtil.h"


TriangleSample::TriangleSample() {
}

TriangleSample::~TriangleSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}

void TriangleSample::Init() {
    LOGCATE("TriangleSample::Draw");

    //脚本

    // layout (location = 0) in vec4 aPosition;
    // layout(qualifier1, qualifier2 = value, ...) variable definition
    // 其中qualifier表示具体的修饰符，最常见的就是代码中的“location”，可以理解为变量的位置。
    char vShaderStr[] =
            "#version 300 es                                          \n"
            "layout(location = 0) in vec4 a_position;                \n"
            "void main()                                             \n"
            "{                                                       \n"
            "   gl_Position = a_position;              \n"
            "}                                         \n";

    char vShaderStr1[] =
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
    LOGCATE("TriangleSample::Draw");
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr1, fShaderStr, m_VertexShader,
                                          m_FragmentShader);
    if (m_ProgramObj) {
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    }
    LOGCATE("TriangleSample::Draw");

}

void TriangleSample::Draw(int screenW, int screenH) {
    LOGCATE("TriangleSample::Draw");
    LOGCATE("TriangleSample::Draw，screenW= %d", screenW);
    LOGCATE("TriangleSample::Draw，screenH = %d", screenH);
    //三角形坐标
    GLfloat vVertices[] = {
            0.5f, 0.5f, 0.0f, // top
            -0.5f, -0.5f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f  // bottom right
    };

    if (m_ProgramObj == 0)
        return;

    // Use the program object
    glUseProgram(m_ProgramObj);

    UpdateTransformMatrix(m_MVPMatrix, screenW, screenH);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Load the vertex data
    //index：表示着色器中要接收数据的变量的引用。即着色器中的layout。
    //size：表示每一个顶点属性需要用多少个数组元素表示。比如一个3维坐标是xyz表示，那么size就是3，即3个数可以表示一个点坐标。
    //type：每一个数组元素的格式是什么，比如GL_HALF_FLOAT,GL_FLOAT,GL_DOUBLE等。
    //normalized：是否需要归一化，即是否需要将数据范围映射到-1到1的区间内。
    //stride：步长，一个重要概念，表示前一个顶点属性的起始位置到下一个顶点属性的起始位置在数组中有多少字节。如果传0，则说明顶点属性数据是紧密挨着的。
    //
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);

    //这就是一个开关方法，就是打开着色器中 index = 0 这个变量，
    glEnableVertexAttribArray(0);


    //
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

void TriangleSample::LoadImage(NativeImage *pImage) {

}


void TriangleSample::Destroy() {

}

void TriangleSample::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {

    float ratio = (float) screenW / screenH;  //关系到左右缩放
    float ratio1 = (float) screenH / screenW; //关系到上下缩放

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
    glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 1.0f, 100.0f);
    glm::mat4 Projection1 = glm::ortho(-1.0f, 1.0f, -ratio1, ratio1, 10.0f, 100.0f);
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 100), // Camera is at (0,0,1), in World Space
            // 相机位置
            glm::vec3(0, 0, 0), // and looks at the origin
            // 观测点位置

            glm::vec3(0, 1,
                      0)  // Head is up (set to 0,-1,0 to look upside-down)is up (set to 0,-1,0 to look upside-down)
            // up向量在xyz上的分量
    );

    // Model matrix : an identity matrix (model will be at the origin)
    // glm::mat4 Model = glm::mat4(1.0f);
    mvpMatrix = Projection1 * View;
}