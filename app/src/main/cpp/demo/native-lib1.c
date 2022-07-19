#include <jni.h>
#include <stdlib.h>
#include <string.h>
//
// Created by shixin on 2022/7/17.
//

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_test1(JNIEnv *env, jobject thiz) {

    //获取name属性，然后修改

    //获取jClass
    jclass jclz = (*env)->GetObjectClass(env, thiz);

    //获取jfield
    //name 获取哪个属性的属性名
    //sig 属性的签名
    jfieldID j_fid = (*env)->GetFieldID(env, jclz, "name", "Ljava/lang/String;");

    //获取属性
    jstring j_str = (*env)->GetObjectField(env, thiz, j_fid);

    //打印字符串
    char *c_str = (*env)->GetStringUTFChars(env, j_str, NULL);

    //    void        (*SetObjectField)(JNIEnv*, jobject, jfieldID, jobject);
    jstring mName = (*env)->NewStringUTF(env, "test1");
    (*env)->SetObjectField(env, thiz, j_fid, mName);

}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_test2(JNIEnv *env, jclass clazz) {

    //获取fieldid
    jfieldID j_fid = (*env)->GetStaticFieldID(env, clazz, "age", "I");

    jint old_age = (*env)->GetStaticIntField(env, clazz, j_fid);

    //jint 不需要转
    old_age += 12;
    (*env)->SetStaticIntField(env, clazz, j_fid, old_age);
}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_callAddMethod(JNIEnv *env, jobject thiz) {

    //调用java 方法

    jclass j_clz = (*env)->GetObjectClass(env, thiz);

    //获取methodid
    jmethodID j_mid = (*env)->GetMethodID(env, j_clz, "add", "(II)I");

    jint sum = (*env)->CallIntMethod(env, thiz, j_mid, 2, 3);

}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_callUUIDMethod(JNIEnv *env, jclass clazz) {


    jmethodID j_mid = (*env)->GetStaticMethodID(env, clazz, "getUUID", "()Ljava/lang/String;");

    //    jobject     (*CallStaticObjectMethod)(JNIEnv*, jclass, jmethodID, ...);
    jstring uuid = (*env)->CallStaticObjectMethod(env, clazz, j_mid);

    //jstring -> c_str
    char *c_uuid = (*env)->GetStringUTFChars(env, uuid, NULL);

    //用完之后要回收，不然一直在内存当中
    (*env)->ReleaseStringUTFChars(env, uuid, c_uuid);
}


//常量指针和指针常量
JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_testConst(JNIEnv *env, jobject thiz) {
    //常量指针
    int number1 = 100;
    int number2 = 200;

    //常量指针,const在 * 之前
    const int *n_p = &number2;

    n_p = &number1;//地址是可以被重新赋值的。
    // *n_p = 100; 值不能修改

    //指针常量
    int *const n_p2 = &number2;

    //指针的值可以修改
    *n_p2 = 300;

    // n_p2 = &number1; 不可以被修改

}


//构建java对象，传递给java层
JNIEXPORT jobject JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_createPoint(JNIEnv *env, jclass clazz) {

    //获取pointclass，name=全类名
    jclass point_clz = (*env)->FindClass(env, "com/shixin/ndk_practice/practicec/Point");

    //构建 java层的Point对象，构造函数的id，构造方法统一用<init>
    jmethodID j_mid = (*env)->GetMethodID(env, point_clz, "<init>", "(II)V");

    //clazz 是test对象不能传
    //构建java层的point对象
    jobject obj = (*env)->NewObject(env, point_clz, j_mid, 2, 3);

    //联系一下 给y赋值，调用set方法，
    jmethodID j_mid1 = (*env)->GetMethodID(env, point_clz, "setX", "(I)V");

    (*env)->CallVoidMethod(env, obj, j_mid1, 33);

    return obj;
}


JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Test1_nativeArr(JNIEnv *env, jobject thiz, jintArray arr) {

    //GetArrayElements 和ReleaseArrayElements成对使用
    jint *data = (*env)->GetIntArrayElements(env, arr, NULL);
    jint buffer[10];
    if (data != NULL) {
        memcpy(buffer, data, sizeof(arr) / sizeof(int));
        (*env)->ReleaseIntArrayElements(env, arr, data, JNI_ABORT);
    }

    //GetArrayRegion
    //GetArrayRegion单独使用，
    //
    //表示事先在C/C++中创建一个缓存区，然后将Java中的原始数组拷贝到缓冲区中去
    (*env)->GetIntArrayRegion(env, arr, 0, sizeof(arr) / sizeof(int), buffer);
}