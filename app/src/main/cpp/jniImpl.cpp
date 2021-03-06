//
// Created by 203 on 2021/8/31.
//

#include "jni.h"
#include "util/LogUtil.h"
#include "render/MyGLRenderContext.h"

#define NATIVE_RENDER_CLASS_NAME  "com/shixin/ndk_practice/opengl/MyNativeRender"
#define NATIVE_LIB_CLASS_NAME     "com/shixin/ndk_practice/opengl/NativeSimpleRenderer"

const GLfloat TRIANGLE_VERTICES[] = {0.0f, 0.5f, 0.0f,
                                     -0.5f, -0.5f, 0.0f,
                                     0.5f, -0.5f, 0.0f};

const char VERTEX_SHADER[] =
        "#version 300 es \n"
        "layout (location = 0) in vec4 vPosition;\n"
        "layout (location = 1) in vec4 aColor;\n"
        "out vec4 vColor;\n"
        "void main() { \n"
        "gl_Position  = vPosition;\n"
        "gl_PointSize = 10.0;\n"
        "vColor = aColor;\n"
        "}\n";

const char FRAGMENT_SHADER[] =
        "#version 300 es \n"
        "precision mediump float;\n"
        "in vec4 vColor;\n"
        "out vec4 fragColor;\n"
        "void main() { \n"
        "fragColor = vColor;\n"
        "}\n";

float color[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
};

GLuint CompileShader(GLenum type, const char *shaderCode) {
    GLint shader = glCreateShader(type);

    if (shader != 0) {
        glShaderSource(shader, 1, &shaderCode, NULL);
        glCompileShader(shader);
        //检测状态
        GLint glResult = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &glResult);
        if (glResult == GL_FALSE) {
            //创建失败
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }
    //创建失败
    return 0;
}

GLint LinkProgram(GLint vertexShader, GLint fragmentShader) {
    GLint program = glCreateProgram();
    if (program != 0) {
        //将顶点着色器加入到程序
        glAttachShader(program, vertexShader);
        //将片元着色器加入到程序中
        glAttachShader(program, fragmentShader);
        //链接着色器程序
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus == GL_FALSE) {
            glDeleteProgram(program);
            return 0;
        }
        return program;
    }
    //创建失败
    return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_Init
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_Init(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance();

}

/*
 * Class:     com_byteflow_app_MyNativeRender
 * Method:    native_UnInit
 * Signature: ()V
 */
JNIEXPORT void JNICALL native_UnInit(JNIEnv *env, jobject instance) {
    MyGLRenderContext::DestroyInstance();
}


JNIEXPORT void JNICALL native_SetImageData(
        JNIEnv *env, jobject instance, jint format, jint width, jint weight, jbyteArray imageData
) {
    int len = env->GetArrayLength(imageData);
    uint8_t *buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte *>(buf));

    delete[] buf;
    env->DeleteLocalRef(imageData);
}

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance()->OnSurfaceCreated();
}

JNIEXPORT void JNICALL
native_OnSurfaceChanged(JNIEnv *env, jobject instance, jint width, jint height) {
    MyGLRenderContext::GetInstance()->OnSurfaceChanged(width, height);
}


JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *env, jobject instance) {
    MyGLRenderContext::GetInstance()->OnDrawFrame();
}


JNIEXPORT void JNICALL surfaceCreated(JNIEnv *env, jobject obj) {
    //创建顶点着色器
    GLint vertexShader = CompileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    if (vertexShader == 0) {
        LOGCATE("loadVertexShader Failed");
        return;
    }
    //创建片段着色器
    GLint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        LOGCATE("loadFragmentShader Failed");
        return;
    }
    GLint program = LinkProgram(vertexShader, fragmentShader);
    //在OpenGLES环境中使用程序片段
    glUseProgram(program);

    //设置背景颜色
    glClearColor(0.5f, 0.5f, 0.5f, 0.5f);
}

JNIEXPORT void JNICALL surfaceChanged(JNIEnv *env, jobject obj, jint width, jint height) {
    glViewport(0, 0, width, height);
}

JNIEXPORT void JNICALL onDrawFrame(JNIEnv *env, jobject obj) {
    //把颜色缓冲区设置为我们预设的颜色
    glClear(GL_COLOR_BUFFER_BIT);
    //准备坐标数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, TRIANGLE_VERTICES);
    //启用顶点的句柄
    glEnableVertexAttribArray(0);

    //绘制三角形颜色
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, false, 0, color);

    //绘制
    glDrawArrays(GL_TRIANGLES, 0, 3);
    //禁止顶点数组的句柄
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}




#ifdef __cplusplus
}
#endif

static JNINativeMethod g_RenderMethods[] = {
        {"native_Init",             "()V",      (void *) (native_Init)},
        {"native_OnUnInit",         "()V",      (void *) (native_UnInit)},
        {"native_SetImageData",     "(III[B)V", (void *) (native_SetImageData)},
        {"native_OnSurfaceCreated", "()V",      (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged", "(II)V",    (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",      "()V",      (void *) (native_OnDrawFrame)},
};


/**
 * 动态注册
 */
JNINativeMethod methods[] = {
        {"surfaceCreated", "()V",   (void *) surfaceCreated},
        {"surfaceChanged", "(II)V", (void *) surfaceChanged},
        {"onDrawFrame",    "()V",   (void *) onDrawFrame}
};


static int
RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    LOGCATE("register");
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        LOGCATE("registernativemethod fail. clazz == null");
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, methods, methodNum < 0)) {
        LOGCATE("register native method fail.");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}


static void UnregisterNativeMethods(JNIEnv *env, const char *className) {
    LOGCATE("unregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        LOGCATE("UnregisterNativeMethods fail. clazz == null");
    }
    if (env != nullptr) {
        env->UnregisterNatives(clazz);
    }
}

//call this func when loading lib
extern "C" jint JNI_OnLoad(JavaVM *jvm, void *p) {
    LOGCATE("===== JNI_OnLoad ------");
    jint jniRet = JNI_ERR;
    JNIEnv *env = nullptr;
    if (jvm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return jniRet;
    }

    jint regRet = RegisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME, g_RenderMethods,
                                        sizeof(g_RenderMethods) /
                                        sizeof(g_RenderMethods[0]));
    if (regRet != JNI_TRUE) {
        return JNI_ERR;
    }

    regRet = RegisterNativeMethods(env, NATIVE_LIB_CLASS_NAME, methods,
                                   sizeof(methods) /
                                   sizeof(methods[0]));
    if (regRet != JNI_TRUE)
    {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

extern "C" void JNI_OnUnload(JavaVM *jvm, void *p) {
    JNIEnv *env = nullptr;
    if (jvm->GetEnv((void **) (env), JNI_VERSION_1_6) != JNI_OK) {
        return;
    }
    UnregisterNativeMethods(env, NATIVE_RENDER_CLASS_NAME);
    UnregisterNativeMethods(env, NATIVE_LIB_CLASS_NAME);

}