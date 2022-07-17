//
// Created by 203 on 2021/9/2.
//

//用来打一个标记，在c编译的时候会把头文件copy到你引入的地方，不管是重复引入还是
//相互引入都只会copy一次
#ifndef NDK_PRACTICE_NATIVE_LIB_H
#define NDK_PRACTICE_NATIVE_LIB_H
#ifdef __cplusplus //如果是c++
//不管是c还是c++,统一采用c的编译方式.因为c里面不允许函数的重载
//但是C++可以，因为和
extern "C" {
#endif
//--------------------------------------------------------------------------------------------------
// JNIEXPORT  JNI关键字，标记该方法可以被外部调用
// JNICALL 标记交给jni调用
// JNIEnv c和java调用的桥梁，里面的方法要搞清
// jobject java 传递下来的对象，就是本项目中的JniSimple 对象
// jclass java传递下来的class对象，就是静态方法传下来的对象
JNIEXPORT void JNICALL surfaceCreated(JNIEnv *, jobject);

JNIEXPORT void JNICALL surfaceChanged(JNIEnv *, jobject, jint, jint);

JNIEXPORT void JNICALL onDrawFrame(JNIEnv *, jobject);


//--------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL native_Init(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_UnInit(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_SetImageData(JNIEnv *, jobject, jint, jint, jint, jbyteArray);

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *, jobject, jint, jint);

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *, jobject);
//--------------------------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif
#endif //NDK_PRACTICE_NATIVE_LIB_H

