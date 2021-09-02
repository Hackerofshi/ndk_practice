//
// Created by 203 on 2021/9/2.
//

#ifndef NDK_PRACTICE_NATIVE_LIB_H
#define NDK_PRACTICE_NATIVE_LIB_H
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL surfaceCreated(JNIEnv *, jobject);

JNIEXPORT void JNICALL surfaceChanged(JNIEnv *, jobject, jint, jint);

JNIEXPORT void JNICALL onDrawFrame(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_Init(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_UnInit(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_SetImageData(JNIEnv *, jobject, jint, jint, jint,jbyteArray);

JNIEXPORT void JNICALL native_OnSurfaceCreated(JNIEnv *, jobject);

JNIEXPORT void JNICALL native_OnSurfaceChanged(JNIEnv *, jobject, jint, jint);

JNIEXPORT void JNICALL native_OnDrawFrame(JNIEnv *, jobject);


#ifdef __cplusplus
}
#endif
#endif //NDK_PRACTICE_NATIVE_LIB_H
