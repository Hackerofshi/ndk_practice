//
// Created by shixin on 2022/7/27.
//

#include <cstdarg>
#include <iostream>
#include "TestClass_static_friend.h"

using namespace std;

//可变参数
void sum(int count, ...) {
    va_list vp;

    //可变参数开始方法，count代表从哪里开始
    va_start(vp, count);

    int number = va_arg(vp, int);

    cout << number << endl;

    //读取下一个
    number = va_arg(vp, int);
    cout << number << endl;

    //读取下一个,超出了默认是0
    number = va_arg(vp, int);
    cout << number << endl;
}

int sum1(int count, ...) {
    va_list vp;

    //可变参数开始方法，count代表从哪里开始
    va_start(vp, count);

    int sum = 0;
    for (int i = 0; i < count; ++i) {
        sum += va_arg(vp, int);
    }
    //结尾释放内存
    va_end(vp);

    return sum;
}

void testVaarg() {
    int number = sum1(5, 1, 2, 4);
    cout << number << endl;
}

//2.静态属性
class Student {
public:
    char *name;
    int age;
    //静态常量
    static int tag;

    Student() {
        tag = 12;
    }

public:
    static void change() {
        tag += 12;
    }

    void change1() {
        this->change();
    }
};

// 静态属性在 c++ 中必须要初始化，初始化必须这么写
int Student::tag = 12;

// 静态 可以直接用类名去操作 ::
//      静态的属性必须要初始化 （实现）
//      静态的方法只能去操作静态的属性或者方法

void testStatic() {
    Student stu;

    stu.tag = 12;

    //c++操作静态语法
    Student::tag += 12;
    Student::change();

    stu.change1();
    cout << Student::tag << endl;


}

//3.对象的大小
class A {
public:
    double A;
    int b;
    char c;
};


class B {
public:
    double A;
    int b;
    char c;
    static double d;

};

class C {
public:
    double a;
    int b;
    char c;

    C(int a) {
        this->a = a;
    }

public:
    double getA() const {
        return a;
    }
};

//1.对象的大小和结构体的计算方式类似
//2. static 静态变量和方法并没有算到类的大小中
//栈 堆 全局 （静态，常量，字符串），代码区，类的大小只与普通属性有关系
void testClassMemory() {
    cout << "A的大小" << sizeof(A) << endl;
    cout << "B的大小" << sizeof(B) << endl;
    cout << "C的大小" << sizeof(C) << endl;

    C c1(12);
    C c2(24);

    cout << c1.getA() << endl;
    cout << c2.getA() << endl;
}

//this 指针，代表当前对象，因为类的方法放在代码区，大家一起共享的，所以要有this做区分

class Student1 {
public:
    char *name;
    int age;
public:
    // this = const Student *const this
    // 第一个 const ：常量指针，代表值不能修改
    // 第二个 const ：指针常量，代表指针的地址不能修改

    void change() const { //const 在（）之后主要用来限制this关键字
      //  this->age = 12;// 不能对类的属性进行修改
    }
};