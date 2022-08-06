## 1.头文件引入

1. 头文件引入相当于复制代码

## 2.c 和 C++ jniEnv的区别

# if defined(__cplusplus)

typedef _JNIEnv JNIEnv; typedef _JavaVM JavaVM;

# else

typedef const struct JNINativeInterface* JNIEnv; typedef const struct JNIInvokeInterface* JavaVM;

# endif

## 3.cjni基本数据类型

jniEnv里面的方法

jint jstring jdouble

### 字符串要进行回收

### 方法签名

### 常量指针和指针常量

常量指针：const在*之前，指针的地址可以修改，值不能修改 指针常量：const在*之后

## 4.Native 构建对象如何与java层对应

1 open.cv mat.java -> mat.cpp
```
public class Mat { //java对象

public final long nativeObj;

    public Mat(){
        nativeObj = n_Mat(rows,cols,type); //c++创建对象，返回一个long (c++首地址),
        //下次操作，传回首地址，就可以找到原来的对象
    }

    public static native long n_Mat(int rows,int cols,int type);
}
```
2 Parcel为了实现共享内存

## 5.native 层出错了，没办法在java层try

1 处理方式一般两种，一种补救， 2第二种就是抛出异常对象给java

## 6.cpp文件中类不能重复定义

## 7.GetArrayElements和GetByteArrayRegion的区别

### GetArrayElements

GetArrayElements 和ReleaseArrayElements成对使用

jbyte* data = env->GetByteArrayElements(array, NULL);    

if (data != NULL) { memcpy(buffer, data, len);        
env->ReleaseByteArrayElements(array, data, JNI_ABORT); }

### GetArrayRegion

GetArrayRegion单独使用，

表示事先在C/C++中创建一个缓存区，然后将Java中的原始数组拷贝到缓冲区中去

env->GetByteArrayRegion(array, 0, len, buffer); 1 优点

只需要一个JNI调用而不是两个, 减少开销. 不需要对原始数据进行限制或者额外的拷贝数据 减少开发者的风险(不会有在某些出错后忘记释放的风险)



# c++类型转换
https://www.cnblogs.com/Allen-rg/p/6999360.html

1.1 static_cast 静态转换  用于基本数据类型之间的转换，如把int转换成char
1.2 const_cast 常量转换 用于修改常量的值
1.3 reinterpret_cat 强制类型转换 ，用于转换任意类型
1.4 dynamic_cast 动态转换 ，更安全，转换成功返回类型，失败返回空 ，
必须要包含多态类型和 static_cast 很类似，但是更安全



2. 异常处理
   https://www.cnblogs.com/QG-whz/p/5136883.html

## 相关学习项目
https://github.com/yangkun19921001/AVEditor
https://github.com/yangkun19921001/AVSample

## jni 
https://blog.csdn.net/tkwxty/article/details/103539392 

##  __attribute__ 机制
https://blog.csdn.net/weaiken/article/details/88085360