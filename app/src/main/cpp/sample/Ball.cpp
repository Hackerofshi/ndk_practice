//
// Created by shixin on 2022/10/28.
//

#include "Ball.h"

#define GET_STR(x) #x


Ball::Ball() {
    //球以(0,0,0)为中心，以R为半径，则球上任意一点的坐标为
    // ( R * cos(a) * sin(b),y0 = R * sin(a),R * cos(a) * cos(b))
    // 其中，a为圆心到点的线段与xz平面的夹角，b为圆心到点的线段在xz平面的投影与z轴的夹角
    float step = 1.0;
    std::list<float> values;
    float r1, r2; //相邻的两个半径
    float h1, h2; // y轴的高度
    float sinX, cosX;
    double v = MATH_PI / 180.0;

    for (float i = -90; i < 90 + step; i += step) {
        r1 = cos(i * v);
        h1 = sin(i * v);

        r2 = cos((i + step) * v);
        h2 = sin((i + step) * v);


        // 固定纬度, 360 度旋转遍历一条纬线
        float step2 = step * 2;
        for (float j = 0.0; j < 360.0; j += step2) {
            cosX = cos(j * v);
            sinX = sin(j * v);

            values.push_back(r2 * cosX);
            values.push_back(h2);
            values.push_back(r2 * sinX);

            values.push_back(r1 * cosX);
            values.push_back(h1);
            values.push_back(r1 * sinX);
        }
    }

    gLfloat = new GLfloat[values.size()];
    int i = 0;
    for (float &value: values) {
        gLfloat[i] = value;
        i++;
    }

    vSize = values.size() / 3;
}

Ball::~Ball() {
    if (m_ProgramObj) {
        //  glDeleteProgram(m_ProgramObj);
        GLUtils::DeleteProgram(m_ProgramObj);
    }
}

void Ball::LoadImage(NativeImage *PImage) {
    GLSampleBase::LoadImage(PImage);
}


void Ball::Init() {
    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "uniform mat4 u_MVPMatrix;                  \n"
            "out  vec4 vColor;                          \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = u_MVPMatrix * a_position; \n"
            "    float color;                           \n"
            "        if(a_position.z>0.0){              \n"
            "            color=a_position.z;            \n"
            "        }else{                             \n"
            "            color=-a_position.z;           \n "
            "        }                                  \n"
            "    vColor=vec4(color,color,color,1.0);    \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "in vec4 vColor;                              \n"
            "out vec4 fragColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vColor;                       \n"
            "}                                           \n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader,
                                          m_FragmentShader);
    if (m_ProgramObj) {
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    }
}

void Ball::Draw(int screenW, int screenH) {
    if (m_ProgramObj == 0) {
        return;
    }

    glEnable(GL_DEPTH_TEST);
    // Use the program object
    glUseProgram(m_ProgramObj);

    UpdateTransformMatrix(m_MVPMatrix, screenW, screenH);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);


    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, gLfloat);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, vSize);
    glDisableVertexAttribArray(0);
}

void Ball::Destroy() {

}

void Ball::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {

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
