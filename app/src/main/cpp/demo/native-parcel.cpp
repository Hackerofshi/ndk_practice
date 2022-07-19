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


