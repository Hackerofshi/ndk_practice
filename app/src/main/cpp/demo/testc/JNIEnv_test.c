//
// Created by shixin on 2022/7/17.
//

#include <stdio.h>
#include <stdlib.h>

typedef const struct JNINativeInterface *JNIEnv;

//模拟一个结构体
struct JNINativeInterface {
    //结构体的方法指针
    char *(*NewStringUTF)(JNIEnv *, char *);

    char (*NewIntArr)(JNIEnv *, char);
};

//结构体方法指针的实现
char *NewStringUTF(JNIEnv *env, char *c_str) {
    //模拟转换流程 ....
    return c_str;
}

char NewIntArr(JNIEnv *env, char c_str) {
    return c_str;
}

char *getString(JNIEnv *env) {
    return (*env)->NewStringUTF(env, "asfdadf");
}

char getString1(JNIEnv *env) {
    return (*env)->NewIntArr(env, 'a');
}

void jnienvtest() {
    struct JNINativeInterface nativeInterface;
    nativeInterface.NewStringUTF = NewStringUTF;

    nativeInterface.NewIntArr = NewIntArr;

    JNIEnv env = &nativeInterface; //一级指针
    JNIEnv *jniEnv = &env; //二级指针

    //吧jniEnv 对象传递给 getString
    char *jstr = getString(jniEnv);
    char str = getString1(jniEnv);



    //jstring 通过jnienv传给java层
    //printf("jstring = %s", jstring);
    //getchar();
}

int max(int x, int y) {
    return x > y ? x : y;
}

int mainfun(void) {
    /* p 是函数指针 */
    int (*p)(int, int) =  &max; // &可以省略
    int a, b, c, d;

    printf("请输入三个数字:");
    scanf("%d %d %d", &a, &b, &c);

    /* 与直接调用函数等价，d = max(max(a, b), c) */
    d = p(p(a, b), c);

    printf("最大的数字是: %d\n", d);

    return 0;
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

int mainfun1(void) {
    int myarray[10];
    /* getNextRandomValue 不能加括号，否则无法编译，因为加上括号之后相当于传入此参数时传入了 int , 而不是函数指针*/
    populate_array(myarray, 10, getNextRandomValue);
    for (int i = 0; i < 10; i++) {
        printf("%d ", myarray[i]);
    }
    printf("\n");
    return 0;
}