#include <jni.h>
#include <string>
#include "native-lib.h"
#include "MyGLRenderContext.h"
#include <EGL/egl.h>
#include <GLES3/gl3.h>


#ifdef ANDROID


#include <android/log.h>

#define LOG_TAG    "HelloTriangle"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

//绑定在surfaceview中
#define NATIVE_RENDER_CLASS_NAME  "com/shixin/ndk_practice/opengl/MyNativeRender"

//绑定在simpleactivity中
#define NATIVE_LIB_CLASS_NAME     "com/shixin/ndk_practice/opengl/NativeSimpleRenderer"


/**
 * 动态注册
 */
JNINativeMethod n_methods[] = {
        {"surfaceCreated", "()V",   (void *) surfaceCreated},
        {"surfaceChanged", "(II)V", (void *) surfaceChanged},
        {"onDrawFrame",    "()V",   (void *) onDrawFrame}
};


static JNINativeMethod g_RenderMethods[] = {
        {"native_OnInit",           "()V",      (void *) (native_Init)},
        {"native_OnUnInit",         "()V",      (void *) (native_UnInit)},
        {"native_SetImageData",     "(III[B)V", (void *) (native_SetImageData)},
        {"native_OnSurfaceCreated", "()V",      (void *) (native_OnSurfaceCreated)},
        {"native_OnSurfaceChanged", "(II)V",    (void *) (native_OnSurfaceChanged)},
        {"native_OnDrawFrame",      "()V",      (void *) (native_OnDrawFrame)},
};


static int
RegisterNativeMethods(JNIEnv *env, const char *className, JNINativeMethod *methods, int methodNum) {
    LOGE("register");
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        LOGE("registernativemethod fail. clazz == null");
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, methods, methodNum < 0)) {
        LOGE("register native method fail.");
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/**
 * 动态注册
 * @param env
 * @return
 */
jint registerNativeMethod(JNIEnv *env) {
    jclass cl = env->FindClass(NATIVE_LIB_CLASS_NAME);

    if ((env->RegisterNatives(cl, n_methods, sizeof(n_methods) / sizeof(n_methods[0]))) < 0) {
        return -1;
    }
    jclass cl1 = env->FindClass(NATIVE_RENDER_CLASS_NAME);
    if ((env->RegisterNatives(cl1, g_RenderMethods,
                              sizeof(g_RenderMethods) / sizeof(g_RenderMethods[0]))) < 0) {
        return -1;
    }
    return 0;
}


static void UnregisterNativeMethods(JNIEnv *env, const char *className) {
    LOGE("unregisterNativeMethods");
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        LOGE("UnregisterNativeMethods fail. clazz == null");
    }
    env->UnregisterNatives(clazz);
}

/**
 * 加载默认回调
 * @param vm
 * @param reserved
 * @return
 */
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }
    //注册方法
    if (registerNativeMethod(env) != JNI_OK) {
        return -1;
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

JNIEXPORT void JNICALL surfaceCreated(JNIEnv *env, jobject obj) {
    //创建顶点着色器
    GLint vertexShader = CompileShader(GL_VERTEX_SHADER, VERTEX_SHADER);
    if (vertexShader == 0) {
        LOGE("loadVertexShader Failed");
        return;
    }
    //创建片段着色器
    GLint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        LOGE("loadFragmentShader Failed");
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

extern "C"
JNIEXPORT jstring JNICALL
Java_com_shixin_ndk_1practice_opengl_NativeSimpleRenderer_test(JNIEnv *env, jobject thiz) {

#if defined(__arm__)
#if defined(__ARM_ARCH_7A__)
#if defined(__ARM_NEON__)
#if defined(__ARM_PCS_VFP)
#define ABI "armeabi-v7a/NEON (hard-float)"
#else
#define ABI "armeabi-v7a/NEON"
#endif
#else
#if defined(__ARM_PCS_VFP)
#define ABI "armeabi-v7a (hard-float)"
#else
#define ABI "armeabi-v7a"
#endif
#endif
#else
#define ABI "armeabi"
#endif
#elif defined(__i386__)
#define ABI "x86"
#elif defined(__x86_64__)
#define ABI "x86_64"
#elif defined(__mips64)  /* mips64el-* toolchain defines __mips__ too */
#define ABI "mips64"
#elif defined(__mips__)
#define ABI "mips"
#elif defined(__aarch64__)
#define ABI "arm64-v8a"
#else
#define ABI "unknown"
#endif


    return (*env).NewStringUTF("Hello from JNI !  Compiled with ABI " ABI ".");
}
