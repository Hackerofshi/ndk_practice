//
// Created by shixin on 2022/7/17.
//
#include <jni.h>
#include <malloc.h>


//结构体和对象
class Parcel {
    char *mData; //char 共享内存首地址
    int mDataPos = 0; //偏移位置,需要初始化，不初始化会报错，
public:
    Parcel() {
        mData = (char *) malloc(1024);
    }

    void writeInt(jint value) {
        //强转成int *
        *reinterpret_cast<int *>(mData + mDataPos) = value;
        //挪动地址，int长度
        mDataPos += sizeof(int);
    }

    void setDataPosition(jint poi) {
        mDataPos = poi;
    }

    jint readInt() {
        int result = *reinterpret_cast<int *>(mData + mDataPos);

        //往后挪地址
        mDataPos += sizeof(int);
        return result;
    }
};
//思考： 怎么写string，怎么读string

// 5 | shixin
//长度 | 字符串  这样存储

extern "C" JNIEXPORT jlong JNICALL
Java_com_shixin_ndk_1practice_practicec_Parcel_nativeCreate(JNIEnv *env, jobject thiz) {
    Parcel *parcel = new Parcel();
    return (jlong) parcel;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Parcel_nativeWriteInt(JNIEnv *env, jobject thiz,
                                                              jlong m_native_ptr, jint value) {
    Parcel *parcel = (Parcel *) m_native_ptr;
    parcel->writeInt(value);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_Parcel_nativeSetDataPosition(JNIEnv *env, jobject thiz,
                                                                     jlong m_native_ptr,
                                                                     jint value) {

    Parcel *parcel = (Parcel *) m_native_ptr;
    parcel->setDataPosition(value);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_shixin_ndk_1practice_practicec_Parcel_nativeReadInt(JNIEnv *env, jobject thiz,
                                                             jlong m_native_ptr) {
    Parcel *parcel = (Parcel *) m_native_ptr;
    return parcel->readInt();
}



union Speed {
    float num_f;
    unsigned char num_c[4];
};

union Speed1 {
    float num_f;
    char num_c[4];
} s1;


//联合体中的属性值都相等
jfloat testUnion(jint a, jint b, jint c,
                 jint d) {
    Speed s{};
    s.num_c[0] = a;
    s.num_c[1] = b;
    s.num_c[2] = c;
    s.num_c[3] = d;
    float f = s.num_f;

    return f;
}

extern "C"
JNIEXPORT jintArray JNICALL
Java_com_anyemonitor_jni_NDKBridge_floatToInt(JNIEnv *env, jobject thiz, jfloat f) {
    s1.num_f = f;
    jintArray array = env->NewIntArray(4);
    jint j;
    jint buff[4];
    for (j = 0; j < 4; j++) {
        buff[j] = s1.num_c[j];
    }
    (env)->SetIntArrayRegion( array, 0, 4, buff);
    return array;
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
