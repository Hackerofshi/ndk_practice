//
// Created by shixin on 2022/7/17.
//

#include <jni.h>
#include <stdlib.h>


#ifdef ANDROID

#include <android/log.h>
#include <string.h>

#define LOG_TAG    "TEST"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif


// 比较函数，用于升序排序
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_sort(JNIEnv *env, jclass clazz, jintArray arr) {

    // 对 jarray 进行排序 （sort）
    jint *intArray = (*env)->GetIntArrayElements(env, arr, NULL);
    //获取数组的长度
    int length = (*env)->GetArrayLength(env, arr);

    //void qsort(void* __base, size_t __nmemb, size_t __size,
    // int (*__comparator)(const void* __lhs, const void* __rhs));
    // 第一个参数：void* 数组的首地址
    // 第二个参数：数组的大小长度
    // 第三个参数：数组元素数据类型的大小
    // 第四个参数：数组的一个比较方法指针（Comparable）
    qsort(intArray, length, sizeof(int), compare);


    // 同步数组的数据给 java 数组 intArray 并不是 jarray ，
    // 可以简单的理解为 ,不然不会同步结果
    // 最后一个参数
    // 0 : 既要同步数据给 jarray ,又要释放 intArray
    // JNI_COMMIT:  会同步数据给 jarray ，但是不会释放 intArray
    // JNI_ABORT: 不同步数据给 jarray ，但是会释放 intArray
    (*env)->ReleaseIntArrayElements(env, arr, intArray, JNI_ABORT);
}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_localRef(JNIEnv *env, jclass clazz) {
    //在native层构建的Java对象，你不用了该怎么管理
    //native层开辟的内存由谁管理，你能开辟多大

    //动态开辟的内存
    jclass str_clz = (*env)->FindClass(env, "java/lang/String");
    jmethodID init_mid = (*env)->GetMethodID(env, str_clz, "<init>", "()V");

    jobject j_str = (*env)->NewObject(env, str_clz, init_mid);

    //假设还有100行代码

    // jobject 不要再使用了，要回收 javaGC 的源码,
    //
    (*env)->DeleteLocalRef(env, j_str);


    // 删除了就不能再使用了，C 和 C++ 都需要自己释放内存
    // （静态开辟的不需要，动态开辟的需要）
}


// 全局变量
jstring globalStr;

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_globalRef(JNIEnv *env, jclass clazz,
                                                            jstring str) {
    // 保存全局变量，其他方法需要用到
    globalStr = (*env)->NewGlobalRef(env, str);

    // NewWeakGlobalRef (java 中的软引用很像) 无法保证对象不为空
}

JNIEXPORT jstring JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_getGlobalRef(JNIEnv *env, jclass clazz) {
    return globalStr;
}

JNIEXPORT jstring JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_deleteGlobalRef(JNIEnv *env, jclass clazz) {
    (*env)->DeleteGlobalRef(env, globalStr);
}


// 全局静态缓存，在构造函数中初始化的时候会去缓存
static jfieldID f_name_id = NULL;
static jfieldID f_name1_id = NULL;
static jfieldID f_name2_id = NULL;

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_staticLocalCache(JNIEnv *env, jclass clazz,
                                                                   jstring str) {

    // name属性 赋值操作
    static jfieldID f_id = NULL;
    // 局部缓存，这个方法会被多次调用，不需要反复的去获取 jfieldID
    // OpenCV WebRtc
    if (f_id == NULL) {
        f_id = (*env)->GetStaticFieldID(env, clazz, "name", "Ljava/lang/String;");
    } else {
        printf("fieldID is not null\n");
    }

    (*env)->SetStaticObjectField(env, clazz, f_id, str);
}


JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_initStaticCache(JNIEnv *env, jclass clazz) {
    // 初始化全局静态缓存
    f_name_id = (*env)->GetStaticFieldID(env, clazz, "name", "Ljava/lang/String;");
    f_name1_id = (*env)->GetStaticFieldID(env, clazz, "name1", "Ljava/lang/String;");
    f_name2_id = (*env)->GetStaticFieldID(env, clazz, "name2", "Ljava/lang/String;");

}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestArray_exeception(JNIEnv *env, jclass clazz) {
    // 假设现在想给 ，name 赋值 name3
    jfieldID f_id = (*env)->GetStaticFieldID(env, clazz, "name1", "Ljava/lang/String;");

    // 好几种方式
    // 1. 补救措施 ，name3 我拿 name
    // 1.1 有没有异常
    jthrowable throwable = (*env)->ExceptionOccurred(env);
    /*if (throwable){
        // 补救措施，先把异常清除
        printf("有异常");
        // 清除异常,然后才能重新获取属性
        (*env)->ExceptionClear(env);
        // 重新获取 name 属性
        f_id = (*env)->GetStaticFieldID(env, clazz, "name", "Ljava/lang/String;");
    }*/

    // 2. 想给 java 层抛一个异常
    if (throwable) {
        // 清除异常
        (*env)->ExceptionClear(env);
        // Throw 抛一个 java 的 Throwable 对象
        jclass no_such_clz = (*env)->FindClass(env, "java/lang/NoSuchFieldException");
        (*env)->ThrowNew(env, no_such_clz, "NoSuchFieldException name3");

        return;// 记得 return
    }

    jstring name = (*env)->NewStringUTF(env, "Darren");
    (*env)->SetStaticObjectField(env, clazz, f_id, name);
}

//--------------------------------------------------------------------------------------------------
//数组内存，传递数组其实为传递的首地址
void print(int *arr, int length) {
    for (int i = 0; i < length; ++i) {
        LOGI("%d  ", arr[i]);
    }
}

void testArr() {
    int a;//告诉c和c++编译器开辟一块连续大小4字节的内存空间
    int arr[] = {1, 2, 3, 4, 5, 6};
    //求数组的大小
    int size = sizeof(arr) / sizeof(int);

    LOGI("size = %d\n", size);
    print(arr, size);
}


void testArr1() {
    int a;//告诉c和c++编译器开辟一块连续大小4字节的内存空间
    int arr[] = {1, 2, 3, 4, 5, 6}; // arr 数据类型的内存大小空间 24

    a = 10;
    // 12 , 16 数组的地址往后挪一位就是 4个字节
    LOGI("%d, %d", arr, arr + 1);
    //12 , 36   数组的长度为24，地址+1，移动到指针内存末尾
    LOGI("%d, %d", &arr, &arr + 1);

}
//--------------------------------------------------------------------------------------------------
//从 java数组到c数组
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

    //只需要一个JNI调用而不是两个, 减少开销.
    //不需要对原始数据进行限制或者额外的拷贝数据
    //减少开发者的风险(不会有在某些出错后忘记释放的风险)
}

//--------------------------------------------------------------------------------------------------

int compare_strings(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int main1() {
    const char *arr[] = {"apple", "orange", "banana", "pear"};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);

    qsort(arr, arr_size, sizeof(char *), compare_strings);

    for (int i = 0; i < arr_size; i++) {
        printf("%s ", arr[i]);
    }
    printf("\n");

    return 0;
}