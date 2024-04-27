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

typedef unsigned int GLuint1;

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


int max1(int x, int y) {
    return x > y ? x : y;
}



void testMax() {
    int (*p)(int, int) = &max1;
    int a = 3;
    int b = 4;
    int c = 5;
    int d = 6;

    int s = p(a, b);

    LOGI("最大的数字是: %d\n", s);
}

// 回调函数
void populate_array1(int *array, size_t arraySize, int (*getNextValue)(void)) {
    for (size_t i = 0; i < arraySize; i++)
        array[i] = getNextValue();
}

// 获取随机值
int getNextRandomValue1(void) {
    return rand();
}


void testFun() {
    int myarr[10];
    populate_array1(myarr, 10, getNextRandomValue1);
    for (int i = 0; i < 10; i++) {
        LOGI("%d ", myarr[i]);
    }
    printf("\n");
}


JNIEXPORT void JNICALL
Java_com_shixin_ndk_1practice_practicec_TestPoniter_test3(JNIEnv *env, jclass clazz) {


}


//值传递,无法修改值
void change(int num) {
    num = 300;
    LOGI("%p\n", &num);
}

//引用传递,修改指针指向的地址
void change1(int *num1) {
    *num1 = 300;
}

void test1() {
    int a = 100;
    //修改a的值
    a = 200;

    change(a); //a=200 a 变量的值赋值给了num 变量，num在另一个函数中，
    // num是一个新的变量

    change1(&a); //a = 300 把a的地址传给了num1，改变地址的指向就改变了值
}


//指针间接赋值的意义
void testPoint() {
    int a = 100;
    //修改a的值,同个方法里直接赋值
    a = 200;

    //通过指针，其实就是为了在其他方法中间接赋值
    //开发何种，指针出现的意义，一，通过方法是修改值
    change1(&a);

    //c和java方法不同所在，c可以在传对象的地址在方法里面去赋值，java是返回一个对象，返回值

    int b = 10;

    int *d = &b;
    int **c = &d;

    //多级指针
    LOGI("%d", **c);

}


//1.一般不要修改指针的地址
void test_point1() {
    char *name = "test11";
    char *tempName = name; //赋值给临时变量进行操作
    for (int i = 0; i < 6; ++i) {
        LOGI("%c", *name);
        //name++; //错误操作



        tempName++; //指针++
    }

    //name++ 后
    LOGI("%s", name); //此时操作就会报错
}

void set(char **str) {
    *str = "test";
    //指针变量的赋值改变的指针的指向；
}

void test_point2() {
    char *name = NULL;
    set(&name);  //此时可以操作，&name 是取出地址，地址不是null
    //强调一个概念，指针变量和指针所指向的变量是两个不同的概念


    LOGI("%s", name);

    char name1 = NULL;
    char *p = &name1;

    int *name2 = NULL;
    int **p1 = &name2;
}

//const 关键字 c和c++会有区别
//const 在c里面其实还是可以修改，
void test_point3() {
    //常量，通过指针还可以赋值
    const int number = 100;

    //常量指针
    int *const num1 = &number;
    *num1 = 2000;  //值可以改

    const int *const numerP = &number; //d都不能改
}

typedef struct FILE {
    char *fileName;
    int len;
} File;

//二级指针的内存模式
//指针数组：数组指针指向的是数组元素的首地址
void test_point4() {
    char *name = "test11";
    //二级指针，可以看出二维数组
    char **nameP = &name;

    //定义一个File* 数组
    File *file[2] = {{"int.mp4"},
                     {"int1.mp4"}};
    File **file1 = file; //另外一种定义方式，（指针数组）（二级指针），结果相同
}

void prints(char **strs, int len) {
    for (int i = 0; i < len; ++i) {
        LOGI("%s", strs[i]); // []也相当于挪动指针
        // LOGI("%s", *(strs+1)); 相当于这样
    }
}

void prints1(char strs[][10], int len) {
    for (int i = 0; i < len; ++i) {
        LOGI("%s", strs[i]); // []也相当于挪动指针
        // LOGI("%s", *(strs+1)); 相当于这样
    }
}

void test_point5() {
    char *name[10] = {"test1", "test1", "test1"}; //静态常量区

    //打印一下
    LOGI("%s", name[0]);
    LOGI("%s", name[1]);
    LOGI("%s", name[2]);

    char **nameP = name;

    prints(name, 10);

    //第二种定义方式
    char name1[10][10] = {"test1", "test2", "test2"}; //从静态常量区，copy到栈buffer里面

    //报错，
    //指针的步长不一样
    //prints(name1, 10);

    prints1(name1, 10);


    //第三种方式，最常见的一种方式，动态开辟内存
    int number = 3;
    char **params = malloc(sizeof(char *) * number);
    //开辟一维数组
    for (int i = 0; i < number; ++i) {
        params[i] = malloc(sizeof(char) * 100);
        //写一些数据
        sprintf(params[i], "i = %d", i);
    }

    prints(params, number);

    //开辟后要释放
    for (int i = 0; i < number; ++i) {
        //先释放一维数组
        if (params[i] != NULL)
            free(params[i]);
    }

    //然后再释放params 二维数组（也可以看成一维数组，
    // 里面放的是一级指针）
    if (params != NULL) {
        free(params);
        params = NULL;
    }
}

void initParams(char ***params) { //三级指针，一般就用到二级指针
    //开辟内存去赋值
    int number = 3;
    char **tempParams = malloc(sizeof(char *) * number);
    //开辟一维数组
    for (int i = 0; i < number; ++i) {
        tempParams[i] = malloc(sizeof(char) * 100);
        sprintf(tempParams[i], "i = %d", i); //赋值
    }
    *params = tempParams;
}


//多级指针的用法，
void test_point6() {
    //我想给字符串数组开辟内存，开辟内存写到另一个方法
    char **params = NULL;
    initParams(&params);

    //初始化后会报错
}
