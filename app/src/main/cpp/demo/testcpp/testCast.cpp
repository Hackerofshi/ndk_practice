//
// Created by shixin on 2022/7/30.
//
#include <iostream>

using namespace std;

class Person {
public:
    string name;
    int age;

public:
    Person(string name, int age) {
        this->name = name;
        this->age = age;
    }
};

class Student : public Person {
public:
    Student(string name, int age) : Person(name, age) {
        cout << "ff" << endl;
    }
};

class Worker : public Person {

};

void testcast() {
    double number1 = 20.02;
    // 直接转换
    // int number2 = number1;
    // 1. 用于基本数据类型之间的转换，如把int转换成char
    // int number2 = static_cast<int>(number1);
    // cout << number2 << endl;
    // 2. 把类型转换成另一种类型，用于类层次结构中基类和
    // 派生类之间指针或引用的转换
    // Student *stu = new Student("Darren",24); // jobejct -> objectArray
    // Person *person = stu;
    // Person *person = static_cast<Person *>(stu);

    //
    Person person = Person("Darren", 24);
    // 转成Student
    // Student stu = person;

    cout << person.name.c_str() << " , " << person.age << endl;

}

void testcast1() {
    const Person *person = new Person("Darren", 24);
    // person->age  =23;

    cout << person->name.c_str() << " , " << person->age << endl;

    // person->age = 24;
    // const_cast
    Person *person1 = const_cast<Person *>(person);
    person1->age = 25;
    person1->name = "Jack";
    //改变一个常量的指针，可以修改上面的值
}

// reinterpret_cast
void testcast2() {
    Person *person = new Person("Darren", 24);
    // 转成Student
    // Student stu = person;
    // Student *stu = static_cast<Student *>(person); 子类转成父类


    //地址
    long mPtr = (long) person;

    Student *stu = reinterpret_cast<Student *>(mPtr);

    // 除了字符类，各种类型的转换  long -> 对象的指针* ，
    //
    // 用到 reinterpret_cast
    // 与 static_cast 区别在于 static_cast 一般用于转换有继承关系的类型
    // reinterpret_cast 也能转换继承关系的类型
    Student *stu = reinterpret_cast<Student *>(person);

    cout << stu->name.c_str() << " , " << stu->age << endl;

    getchar();
}


// dynamic_cast  动态转换类型，
//必须包含多态类类型
void testcast3() {
    Student *stu = new Student("", 24);

    Person *worker = dynamic_cast<Person *>(stu);

    getchar();
}

//自定义异常
class Exception {
public:
    string msg;

public:
    Exception(string msg) {
        this->msg = msg;
    }

public:
    const char *what() {
        return this->msg.c_str();
    }

};

// 异常的处理
void testexception() {
    // c++ 中有自己一套异常的体系，不要去强记
    // 但是 c++ 可以抛任何数据类型 try{}catch(数据类型 变量名){}
    // throw 抛异常

    try {
        int i = -1;

        if (i == 0) {
            throw Exception("出异常了");
        }

        if (i < 0) { //这个类型的异常属于任意类型
            throw 12.5f;
        }
    }
    catch (int number) {
        cout << "捕捉到异常" << number << endl;
    }
    catch (Exception exception) {
        cout << "捕捉到异常:" << exception.what() << endl;
    }
    catch (...) { //捕捉到任意类型的异常
        cout << "捕捉到其他异常:" << endl;
    }


}

// NDK 异常总结
// 1. 在 c++ 层如果是自己写的代码或者调用别人的方法，记得要 try 住，
//  如果不 try 在 java 层 try 是没有意义的，java层无法try住异常
// 2. 如果异常需要往外抛给 java 层，一定要按照java层抛异常的方式
// 3. 如果是自己写的 NDK 层的代码，最好抛自己写的异常，声明异常
// 4. 如果是做 c++/c , 或者是帮 c/c++ 写代码，
// 最好抛系统定义好的异常或者继承系统的异常
// 5. 系统异常的体系  exception 基类
// https://www.cnblogs.com/QG-whz/p/5136883.html
void c_method() throw(Exception, int) {// 如果是自己的写 ，1000 多行
   // throw Exception("异常了");
}


void
arraycopy(JNIEnv
          *env,
          jclass type, jobject
          src,
          jint srcPos,
          jobject
          dest,
          jint destPos, jint
          length) {

// 做一些列的判断 ，是不是数组
// 做一系列的准备工作 ，如果有异常，挂了，程序是会崩掉
// buggly  so库报错 ，如果有抛 java 层的异常，是可以 try ，但是很多情况下并不会抛java层的异常
// 如果没抛 java 层的异常，在 java 层 try 了 依旧会蹦
// src
// dest
// 都是 java 数组，需要类型转换 reinterpret_cast static_cast dynamic_cast(最佳)
// dynamic_cast 但是必须包含多态类型
    jobjectArray src_array = reinterpret_cast<jobjectArray>(src);
    jobjectArray dest_array = reinterpret_cast<jobjectArray>(dest);
    if (src_array !=
        NULL && dest_array
                != NULL) {
        __android_log_print(ANDROID_LOG_ERROR,
                            "TAG", "转换成功");

// 设置值
        for (
                int i = srcPos;
                i < srcPos +
                    length;
                ++i) {
            jobject obj = env->GetObjectArrayElement(src_array, i);
// 放到新的数组里面
            env->
                    SetObjectArrayElement(dest_array, i, obj
            );
        }
// 完毕
    }


    //抛出异常给java
    try {
        c_method();
    } catch (int number) {
        jclass je = env->FindClass("java/lang/Exception");
        env->ThorwNew(je, "位置错误");
    };
}

