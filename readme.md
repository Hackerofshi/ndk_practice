
# 头文件引入

1. 头文件引入相当于复制代码

# c 和 C++ jniEnv的区别

#if defined(__cplusplus)
typedef _JNIEnv JNIEnv;
typedef _JavaVM JavaVM;
#else
typedef const struct JNINativeInterface* JNIEnv;
typedef const struct JNIInvokeInterface* JavaVM;
#endif

## jni基本数据类型 

jniEnv里面的方法

jint
jstring
jdouble

### 字符串要进行回收

### 方法签名

### 常量指针和指针常量

常量指针：const在*之前，指针的地址可以修改，值不能修改
指针常量：const在*之后

## Native 构建对象如何与java层对应

1 open.cv 
mat.java -> mat.cpp

public class Mat { //java对象

public final long nativeObj;

    public Mat(){
        nativeObj = n_Mat(rows,cols,type); //c++创建对象，返回一个long (c++首地址),
        //下次操作，传回首地址，就可以找到原来的对象
    }

    public static native long n_Mat(int rows,int cols,int type);
}

2 Parcel为了实现共享内存



