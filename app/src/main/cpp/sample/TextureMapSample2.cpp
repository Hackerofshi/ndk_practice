//
// Created by shixin on 2022/10/18.
//

#include "TextureMapSample2.h"
#include "../util/GLUtils.h"
#include "../util/LogUtil.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


TextureMapSample2::TextureMapSample2() {
    m_TextureId = 0;
}

TextureMapSample2::~TextureMapSample2() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void TextureMapSample2::Init() {

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

    //解除绑定  解除与纹理的绑定，避免用其他的纹理方法意外地改变这个纹理
    //这里要重点说一下 glBindTexture 函数。
    //
    //它的作用是绑定纹理名到指定的当前活动纹理单元，当一个纹理绑定到一个目标时，
    // 目标纹理单元先前绑定的纹理对象将被自动断开。纹理目标默认绑定的是 0 ，
    // 所以要断开时，也再将纹理目标绑定到 0 就好了。
    //
    //所以在代码的最后调用了 glBindTexture(GL_TEXTURE_2D, 0) 来解除绑定。
    //
    //当一个纹理被绑定时，在绑定的目标上的 OpenGL 操作将作用到绑定的纹理上，并且，对绑定的目标的查询也将返回其上绑定的纹理的状态。
    //
    //也就是说，这个纹理目标成为了被绑定到它上面的纹理的别名，而纹理名称为 0 则会引用到它的默认纹理。所以，
    // 当后续对纹理目标调用 glTexParameteri 函数设置过滤方式，其实也是对纹理设置的过滤方式。
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";


    char vShaderStr1[] =
            "#version 300 es                             \n"
            "layout(location = 0) in vec4 a_position;    \n"
            "layout(location = 1) in vec2 a_texCoord;    \n"
            "out vec2 v_texCoord;                        \n"
            "uniform mat4 u_MVPMatrix;                   \n"
            "void main()                                 \n"
            "{                                           \n"
            "   gl_Position = u_MVPMatrix * a_position;  \n"
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

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr1, fShaderStr, m_VertexShader,
                                          m_FragmentShader);
    if (m_ProgramObj) {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    } else {
        LOGCATE("TextureMapSample::Init create program fail");
    }
}


void TextureMapSample2::Draw(int screenW, int screenH) {
    LOGCATE("TextureMapSample::Draw()");

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLfloat verticesCoords[] = {
            0.5f,  0.5f, 0.0f,  // top right
            0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f,  // top left
    };

    GLfloat textureCoords[] = {
            0.0f, 0.0f,        // TexCoord 0
            0.0f, 1.0f,        // TexCoord 1
            1.0f, 1.0f,        // TexCoord 2
            1.0f, 0.0f         // TexCoord 3
    };

    GLushort indices[] = {0, 1, 3, 1, 2, 3};


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    if (m_ProgramObj == 0)
        return;
    //use the program object
    glUseProgram(m_ProgramObj);


    glm::mat4 model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 View = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);


    projection = glm::perspective(glm::radians(45.0f), (float) screenW / (float) screenH,
                                  0.1f,
                                  100.0f);
    View = glm::translate(View, glm::vec3(0.0f, 0.0f, -3.0f));

    /**
     * 	tmat4x4<T, P> const & m,
	 * 	T angle,  角度
	 * 	tvec3<T, P> const & v //旋转轴 x y z
     */
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));


    glm::mat4 mvpMatrix = projection * View * model;

    // pass them to the shaders (3 different ways)
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &mvpMatrix[0][0]);



    //load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), verticesCoords);

    // Load the texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), textureCoords);


    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //bind the rgba map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);


    // Set the RGBA map sampler to texture unit to 0
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

}

void TextureMapSample2::LoadImage(NativeImage *pImage) {
    LOGCATE("TextureMapSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;

        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void TextureMapSample2::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteTextures(1, &m_TextureId);
        m_ProgramObj = GL_NONE;
    }
}