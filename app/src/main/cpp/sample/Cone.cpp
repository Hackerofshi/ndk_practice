//
// Created by shixin on 2022/9/20.
//

#include "Cone.h"
#include "../glm/gtc/matrix_transform.hpp"

int n = 360;
float height = 2.0f;  //圆锥高度
float radius = 1.0f;  //圆锥底面半径

Cone::Cone() {
    float angDegSpan = 360.0f / n;
    list<float> values;
    values.push_back(0.0f);
    values.push_back(0.0f);
    values.push_back(height);

    for (int i = -100; i < 260; ++i) {
        values.push_back(radius * sin((i * angDegSpan) * MATH_PI / 180));
        values.push_back(radius * cos((i * angDegSpan) * MATH_PI / 180));

        values.push_back(0.0f);
    }


    gLfloat = new GLfloat[values.size()];
    int i = 0;
    for (float &value: values) {
        gLfloat[i] = value;
        i++;
        std::cout << value << " ";
        LOGCATE("-----------%f", value);
    }

    vSize = values.size() / 3;

}

Cone::~Cone() {
    if (m_ProgramObj) {
        //  glDeleteProgram(m_ProgramObj);
        GLUtils::DeleteProgram(m_ProgramObj);
    }
}

void Cone::LoadImage(NativeImage *pImage) {

}

void Cone::Init() {
    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out  vec4 vColor;                          \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = u_MVPMatrix * a_position; \n"
            "    if(a_position.z!=0.0){                 \n"
            "       vColor=vec4(0.0,0.0,0.0,1.0);       \n"
            "    }else{                                 \n"
            "       vColor=vec4(0.9,0.9,0.9,1.0);       \n"
            "    }                                      \n"
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

void Cone::Draw(int screenW, int screenH) {
    if (m_ProgramObj == 0) {
        return;
    }

    // Use the program object
    glUseProgram(m_ProgramObj);

    UpdateTransformMatrix(m_MVPMatrix, screenW, screenH);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Load the vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, gLfloat);
    glEnableVertexAttribArray(0);


    glDrawArrays(GL_TRIANGLE_FAN, 0, vSize);
    glDisableVertexAttribArray(0);

}

void Cone::Destroy() {

}

void Cone::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {

    float ratio = (float) screenW / screenH;


    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 3.0f, 20.0f);
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(1, -10, -4), // Camera is at (5,5,1), in World Space
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
