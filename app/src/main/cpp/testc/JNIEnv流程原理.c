//
// Created by shixin on 2022/7/17.
//

#include <stdio.h>

typedef const struct JNINativeInterface *JNIEnv;

//模拟一个结构体
struct JNINativeInterface {
    //结构体的方法指针
    char *(*NewStringUTF)(JNIEnv *, char *);
};

//结构体方法指针的实现
char *NewStringUTF(JNIEnv *env, char *c_str) {
    //模拟转换流程 ....
    return c_str;
}

char *getString(JNIEnv *env) {
    return (*env)->NewStringUTF(env, "asfdadf");
}


int main() {
    struct JNINativeInterface nativeInterface;
    nativeInterface.NewStringUTF = NewStringUTF;

    JNIEnv env = &nativeInterface; //以及指针
    JNIEnv *jniEnv = &env; //二级指针

    //吧jniEnv 对象传递给 getString
    char *jstring = getString(jniEnv);
    //jstring 通过jnienv传给java层
    //printf("jstring = %s", jstring);
    //getchar();
}