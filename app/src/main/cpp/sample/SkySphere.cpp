//
// Created by shixin on 2022/10/28.
//

#include "SkySphere.h"


SkySphere::SkySphere() {
    rotationMatrix = glm::mat4(1.0f);

    m_TextureId = 0;
    float radius = 2;
    double angleSpan = MATH_PI / 90;// 将球进行单位切分的角度

    std::list<float> alVertix;
    std::list<float> textureVertix;
    for (double vAngle = 0; vAngle < MATH_PI; vAngle = vAngle + angleSpan) {

        for (double hAngle = 0; hAngle < 2 * MATH_PI; hAngle = hAngle + angleSpan) {
            float x0 = (float) (radius * sin(vAngle) * cos(hAngle));
            float y0 = (float) (radius * sin(vAngle) * sin(hAngle));
            float z0 = (float) (radius * cos((vAngle)));

            float x1 = (float) (radius * sin(vAngle) * cos(hAngle + angleSpan));
            float y1 = (float) (radius * sin(vAngle) * sin(hAngle + angleSpan));
            float z1 = (float) (radius * cos(vAngle));

            float x2 = (float) (radius * sin(vAngle + angleSpan) *
                                cos(hAngle + angleSpan));
            float y2 = (float) (radius * sin(vAngle + angleSpan) *
                                sin(hAngle + angleSpan));
            float z2 = (float) (radius * cos(vAngle + angleSpan));

            float x3 = (float) (radius * sin(vAngle + angleSpan) * cos(hAngle));
            float y3 = (float) (radius * sin(vAngle + angleSpan) * sin(hAngle));
            float z3 = (float) (radius * cos(vAngle + angleSpan));

            alVertix.push_back(x1);
            alVertix.push_back(y1);
            alVertix.push_back(z1);
            alVertix.push_back(x0);
            alVertix.push_back(y0);
            alVertix.push_back(z0);
            alVertix.push_back(x3);
            alVertix.push_back(y3);
            alVertix.push_back(z3);

            float s0 = (float) (hAngle / MATH_PI / 2);
            float s1 = (float) ((hAngle + angleSpan) / MATH_PI / 2);
            float t0 = (float) (vAngle / MATH_PI);
            float t1 = (float) ((vAngle + angleSpan) / MATH_PI);

            textureVertix.push_back(s1);// x1 y1对应纹理坐标
            textureVertix.push_back(t0);
            textureVertix.push_back(s0);// x0 y0对应纹理坐标
            textureVertix.push_back(t0);
            textureVertix.push_back(s0);// x3 y3对应纹理坐标
            textureVertix.push_back(t1);

            alVertix.push_back(x1);
            alVertix.push_back(y1);
            alVertix.push_back(z1);
            alVertix.push_back(x3);
            alVertix.push_back(y3);
            alVertix.push_back(z3);
            alVertix.push_back(x2);
            alVertix.push_back(y2);
            alVertix.push_back(z2);

            textureVertix.push_back(s1);// x1 y1对应纹理坐标
            textureVertix.push_back(t0);
            textureVertix.push_back(s0);// x3 y3对应纹理坐标
            textureVertix.push_back(t1);
            textureVertix.push_back(s1);// x2 y3对应纹理坐标
            textureVertix.push_back(t1);
        }
    }

    vSize = alVertix.size() / 3;


    gLfloat = new GLfloat[alVertix.size()];
    int i = 0;
    for (float &value: alVertix) {
        gLfloat[i] = value;
        i++;
    }


    gLTexturefloat = new GLfloat[textureVertix.size()];
    int j = 0;
    for (float &value: textureVertix) {
        gLTexturefloat[j] = value;
        j++;
    }

}

SkySphere::~SkySphere() {
    if (m_ProgramObj) {
        //  glDeleteProgram(m_ProgramObj);
        GLUtils::DeleteProgram(m_ProgramObj);
    }
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void SkySphere::LoadImage(NativeImage *pImage) {
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;

        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }

}


void SkySphere::Init() {
    //create RGBA texture
    //生成一个纹理，将纹理 id 赋值给 m_TextureId
    glGenTextures(1, &m_TextureId);

    //将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    //设置纹理 S 轴（横轴）的拉伸方式为截取
    //设置环绕方向S，截取纹理坐标到[1/2n,1-1/2n]。将导致永远不会与border融合
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    //设置纹理 T 轴（纵轴）的拉伸方式为截取
    //设置环绕方向T，截取纹理坐标到[1/2n,1-1/2n]。将导致永远不会与border融合
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //设置纹理采样方式
    //设置缩小过滤为使用纹理中坐标最接近的一个像素的颜色作为需要绘制的像素颜色
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //设置放大过滤为使用纹理中坐标最接近的若干个颜色，通过加权平均算法得到需要绘制的像素颜色
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    char vShaderStr[] =
            "#version 300 es                             \n"
            "layout(location = 0) in vec4 a_position;    \n"
            "layout(location = 1) in vec2 a_texCoord;    \n"
            "out vec2 v_texCoord;                        \n"
            "uniform mat4 u_MVPMatrix;                   \n"
            "void main()                                 \n"
            "{                                           \n"
            "   gl_Position =  u_MVPMatrix * a_position; \n"
            "   v_texCoord = a_texCoord;                 \n"
            "}                                           \n";

    char fShaderStr[] =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform sampler2D s_TextureMap;                     \n"
            "void main()                                         \n"
            "{                                                   \n"
            "  outColor = texture(s_TextureMap, v_texCoord);     \n"
            "}                                                   \n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader,
                                          m_FragmentShader);

    if (m_ProgramObj) {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    } else {
        LOGCATE("TextureMapSample::Init create program fail");
    }

}

void SkySphere::Draw(int screenW, int screenH) {

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    //use the program object
    glUseProgram(m_ProgramObj);


    UpdateTransformMatrix(m_MVPMatrix, screenW, screenH);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    //bind the rgba map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);


    //load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), gLfloat);

    // Load the texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), gLTexturefloat);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);


    glUniform1i(m_SamplerLoc, 0);
    glDrawArrays(GL_TRIANGLES, 0, vSize);


    glDisableVertexAttribArray(0);
}

void SkySphere::Destroy() {

}

void SkySphere::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {

    float ratio = (float) screenW / screenH;

    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 3.0f, 20.0f);
    glm::mat4 Projection1 = glm::perspective(45.0f, ratio, 1.0f, 300.0f);
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 0.0f), // Camera is at (5,5,1), in World Space
            // 相机位置

            glm::vec3(0, 0, -1), // and looks at the origin
            // 观测点位置

            glm::vec3(0, 1,
                      0)  // Head is up (set to 0,-1,0 to look upside-down)is up (set to 0,-1,0 to look upside-down)
            // up向量在xyz上的分量
    );
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    mvpMatrix = Projection1 * rotationMatrix * View * Model;
}

void SkySphere::setMatrix(float *pArray) {
    LOGCATI("------进入1");

    rotationMatrix[0][0] = pArray[0];
    rotationMatrix[0][1] = pArray[1];
    rotationMatrix[0][2] = pArray[2];
    rotationMatrix[0][3] = pArray[3];
    rotationMatrix[1][0] = pArray[4];
    rotationMatrix[1][1] = pArray[5];
    rotationMatrix[1][2] = pArray[6];
    rotationMatrix[1][3] = pArray[7];
    rotationMatrix[2][0] = pArray[8];
    rotationMatrix[2][1] = pArray[9];
    rotationMatrix[2][2] = pArray[10];
    rotationMatrix[2][3] = pArray[11];
    rotationMatrix[3][0] = pArray[12];
    rotationMatrix[3][1] = pArray[13];
    rotationMatrix[3][2] = pArray[14];
    rotationMatrix[3][3] = pArray[15];
}
