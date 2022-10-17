//
// Created by shixin on 2022/10/17.
//

#include "VAOSample.h"

VAOSample::VAOSample() {

}

VAOSample::~VAOSample() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
    }
}

void VAOSample::LoadImage(NativeImage *pImage) {
    GLSampleBase::LoadImage(pImage);
}

void VAOSample::Init() {
    LOGCATE("VAOSample::Draw");
    char vShaderStr1[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec4 aColor;       \n"
            "out vec4 vTextColor;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   vTextColor  = aColor    ;               \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                              \n"
            "precision mediump float;                     \n"
            "out vec4 fragColor;                          \n"
            "in vec4 vTextColor;                          \n"
            "void main()                                  \n"
            "{                                            \n"
            "   fragColor = vTextColor;                   \n"
            "}                                            \n";
    LOGCATE("TriangleSample::Draw");
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr1, fShaderStr, m_VertexShader,
                                          m_FragmentShader);
    LOGCATE("TriangleSample::Draw");
}

void VAOSample::Draw(int screenW, int screenH) {

    static float triangleVerWithColor[] = {
            0.0f, 0.8f, 0.0f,//顶点
            1.0, 0.0, 0.0,//颜色

            -0.8f, -0.8f, 0.0f,//顶点
            0.0, 1.0, 0.0,//颜色

            0.8f, -0.8f, 0.0f,//顶点
            0.0, 0.0, 1.0,//颜色
    };


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

    //定义vbo的id数组，因为可能要创建多个vbo
    unsigned int VBOs[1];

    //1.创建缓冲区
    // 创建vbo，将创建好的vbo的id存放在vbos数组中
    glGenBuffers(1, VBOs);


    //2.绑定缓冲区到上下文
    //此时上下文绑定VBOs[0]对应的vbo缓冲
    //1.创建的时候通过指定glGenBuffers的type参数即可确定具体的Buffer Objects种类，常见的有以下种类：
    //GL_ARRAY_BUFFER，GL_ELEMENT_ARRAY_BUFFER，GL_SHADER_STORAGE_BUFFER，GL_PIXEL_PACK_BUFFER等，

    // ----其中GL_ARRAY_BUFFER对应的就是VBO，----
    // ----GL_ELEMENT_ARRAY_BUFFER对应的就是EBO。----
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);

    //3.将顶点数据存入vbo缓冲区中
    //
    //size表示传入的数据长度。
    //data就是具体的数据的指针。
    //       usage指定数据的访问模式，模式可选值有GL_STREAM_DRAW,GL_STREAM_READ,GL_STREAM_COPY,GL_STATIC_DRAW,GL_STATIC_READ,GL_STATIC_COPY,
    //       GL_DYNAMIC_DRAW,GL_DYNAMIC_READ, orGL_DYNAMIC_COPY。
    //
    //       STREAM：数据几乎每次被访问都会被修改。
    //       STATIC：数据只被修改一次。
    //       DYNAMIC：数据会被修改多次。

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVerWithColor), triangleVerWithColor,
                 GL_STATIC_DRAW);

    //4.将指定如何解析顶点属性数组
    //指定如何解析顶点属性的数组，注意这里面最后一个参数传的不是原数据地址，而是数据在vbo缓冲区的相对地址
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, (void *) 0);
    //打开着色器中layout为0的输入变量
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, (void*)(3*4));
    glEnableVertexAttribArray(1);



    /*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, triangleVerWithColor);

    //这就是一个开关方法，就是打开着色器中 index = 0 这个变量，
    glEnableVertexAttribArray(0);*/


    //清屏
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //5. 绘制三角形
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //解绑VBO
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //删除VBO，即清空缓冲区
    //glDeleteBuffers(1, VBOs);
}

void VAOSample::Destroy() {

}

void VAOSample::UpdateTransformMatrix(glm::mat4 &mvpMatrix, int screenW, int screenH) {
    GLSampleBase::UpdateTransformMatrix(mvpMatrix, screenW, screenH);
}
