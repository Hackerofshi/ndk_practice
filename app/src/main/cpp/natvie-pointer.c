//
// Created by shixin on 2022/7/18.
//
#include <jni.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef ANDROID


#include <android/log.h>

#define LOG_TAG    "TEST"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestPoniter_test1(JNIEnv *env, jclass clazz) {
    int var_runoob = 10;
    int *p;
    p = &var_runoob;

    LOGI("var_runoob 变量的地址： %p\n", p);


    int *ip;    /* 一个整型的指针 */
    double *dp;    /* 一个 double 型的指针 */
    float *fp;    /* 一个浮点型的指针 */
    char *ch;    /* 一个字符型的指针 */


    getchar();
}

JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestPoniter_test2(JNIEnv *env, jclass clazz) {
    int var = 20; //实际变量声明
    int *ip;

    ip = &var;


    LOGI("var 变量的地址: %p\n", &var);

    /* 在指针变量中存储的地址 */
    LOGI("ip 变量存储的地址: %p\n", ip);

    /* 使用指针访问值 */
    LOGI("*ip 变量的值: %d\n", *ip);

    //在变量声明的时候，如果没有确切的地址可以赋值，为指针变量赋一个 NULL 值是一个良好的编程习惯。赋为 NULL 值的指针被称为空指针。
    //NULL 指针是一个定义在标准库中的值为零的常量。请看下面的程序：
    int *ptr = NULL;

    LOGI("ptr 的地址是 %p\n", ptr);

    //  if (ptr)     /* 如果 p 非空，则完成 */
    //     if (!ptr)    /* 如果 p 为空，则完成 */

}

const int MAX = 3;

void add() {
    int var[] = {10, 100, 200};
    int i, *ptr;
    //指针中数组地址
    ptr = var;

    for (i = 0; i < MAX; ++i) {

        LOGI("存储地址：var[%d] = %p\n", i, ptr);
        LOGI("存储值：var[%d] = %d\n", i, *ptr);
        ptr++;
    }
}

void decre() {
    int var[] = {10, 100, 200};
    int i, *ptr;

    ptr = &var[MAX - 1];
    for (i = MAX; i > 0; i--) {
        LOGI("存储地址：var[%d] = %p\n", i - 1, ptr);
        LOGI("存储值：var[%d] = %d\n", i - 1, *ptr);

        /* 指向下一个位置 */
        ptr--;
    }
}

//指针可以用关系运算符进行比较，如 ==、< 和 >。如果 p1 和 p2 指向两个相关的变量，
// 比如同一个数组中的不同元素，则可对 p1 和 p2 进行大小比较。
void judge() {
    int var[] = {10, 100, 200};
    int i, *ptr;

    /* 指针中第一个元素的地址 */
    ptr = var;
    i = 0;

    while (ptr <= &var[MAX - 1]) {
        LOGI("存储地址：var[%d] = %p\n", i, ptr);
        LOGI("存储值：var[%d] = %d\n", i, *ptr);

        /* 指向上一个位置 */
        ptr++;
        i++;
    }

}


int max(int x, int y) {
    return x > y ? x : y;
}

void testMax() {
    int (*p)(int, int) = &max;
    int a = 3;
    int b = 4;
    int c = 5;
    int d = 6;

    int s = p(a, b);

    LOGI("最大的数字是: %d\n", s);
}

// 回调函数
void populate_array(int *array, size_t arraySize, int (*getNextValue)(void)) {
    for (size_t i = 0; i < arraySize; i++)
        array[i] = getNextValue();
}

// 获取随机值
int getNextRandomValue(void) {
    return rand();
}


void testFun() {
    int myarr[10];
    populate_array(myarr, 10, getNextRandomValue);
    for(int i = 0; i < 10; i++) {
        LOGI("%d ", myarr[i]);
    }
    printf("\n");
}


JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestPoniter_test3(JNIEnv *env, jclass clazz) {


}
