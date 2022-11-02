//
// Created by shixin on 2022/7/18.
//
#ifdef ANDROID

#include <android/log.h>
#include <string.h>
#include <malloc.h>

#define LOG_TAG    "TEST"
#define LOGE(format, ...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, format, ##__VA_ARGS__)
#define LOGI(format, ...)  __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, format, ##__VA_ARGS__)
#else
#define LOGE(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#define LOGI(format, ...)  printf(LOG_TAG format "\n", ##__VA_ARGS__)
#endif

struct MyStruct {
    char *name;
    union {
        int x, r, s;
    };

};

// int32_t ，可以对基本数据类型取别名
typedef int int32;

typedef long long long_64;

typedef struct MyStruct Student;// 给 MyStruct 取一个别名叫做 Student


void testStruct() {
    Student stu = {"Darren"};
    stu.x = 1;

    int32 number1 = 100;

    LOGI("number1 = %d", number1);

    // 获取 Bitmap ，锁定画布 void**
    // void number;// 错误

    // memcpy();// 拷贝内存，为什么用 void*

}

typedef struct MyStruct1 {
    int a;
} MyStruct1;


struct MyStruct2 {
    int a;
} value;

typedef struct Date {
    int s;
} date;

//结构体赋值操作
typedef struct {
    char name[50];
    int age;
    char *className; //定义了一个 className指针，可以赋任何char* 值
    //那结构体的大小怎么算
    date bir;
} MyStudent;


//传地址
void copyTo(MyStudent *from, MyStudent *to) {
    // *to = *from; //这个地方是浅拷贝，两个地方的指针指向同一个堆地址

    //开辟一块内存，然后再进行赋值
    to->className = (char *) malloc(100);
    strcpy(to->className, from->className);


}

void testStruct1() {
    MyStudent stu1 = {"test", 12};


    MyStudent stu2;
    stu2 = stu1; // = 是内容赋值操作，是吧stu1的值，赋值给stu2

    stu1.age = 20;


    strcpy(stu2.name, stu1.name); //赋值的操作


    stu1.className = "asdfasdf"; //将指针指向常量区中的值

    LOGI("%s,%s,%d", stu1.name, stu1.className, stu1.age);

    // strcpy(stu1.className, "fffff");  这样是无法做到，往内存里面拷贝内容

    //结构体套指针，我们必须对结构体指针进行初始化
    //指针指向任何地址，永远只有4个字节
    stu1.className = (char *) malloc(100); //先在堆上开辟内存
    //这样就可以
    strcpy(stu1.className, "fffff");

    MyStudent stu3 = {};
    copyTo(&stu1, &stu3);
    if (stu1.className) {
        free(stu1.className);
    }

    //
    if (stu3.className) {
        free(stu3.className);
    }
}

struct Tea {
    // double salary;  //字节大的元素尽量往前放
    int number; //4      4
    char sex; // 1个字节，但是在内存中占4个字节，字节对齐 8
    int age; // 4个字节     12
    char name[10]; // 10   10+12 = 22,字节对齐所以是24
    date birthday;  // 12  36
};


//指针的偏移量
void testStruct2() {

    struct Tea *stu1 = NULL;
    // &(stu1->name);
    // &(stu1->age);
    //结构体大小偏移量的问题，
    //1.获取内部元素的偏移量
    //&(stu1->name)-(int)stu
    //获取结构体的大小 sizeof(student)
    //3.大小怎么来的，根据每个的偏移量计算出来的，
    //结构体的大小跟内存位置摆放有关，元素的位置不同，内存大小不同
    //结构体的字节对齐，是针对后面的这个数据来看。
    //把最小的字节数的元素尽量往后靠，结构体一旦定义了，就不要轻易挪动
    //因为我们只要知道内存任何一个变量的地址，就知道结构体的内存大小


}