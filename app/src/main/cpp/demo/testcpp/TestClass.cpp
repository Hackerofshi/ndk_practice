//
// Created by shixin on 2022/7/18.
//

#include <iostream>

using namespace std;

class Box {
public:
    double length;
    double breadth;  // 宽度
    double height;   // 高度
    // 成员函数声明
    double get();

    void set(double len, double bre, double hei);
};


double Box::get() {
    return length * breadth * height;
}

void Box::set(double len, double bre, double hei) {
    length = len;
    breadth = bre;
    height = hei;
}

void TestClass1() {
    Box Box1{};        // 声明 BoxTest，类型为 Box
    Box Box2{};        // 声明 Box2，类型为 Box
    Box Box3{};        // 声明 Box3，类型为 Box
    double volume = 0.0;     // 用于存储体积

    // box 1 详述
    Box1.height = 5.0;
    Box1.length = 6.0;
    Box1.breadth = 7.0;

    // box 2 详述
    Box2.height = 10.0;
    Box2.length = 12.0;
    Box2.breadth = 13.0;

    // box 1 的体积
    volume = Box1.height * Box1.length * Box1.breadth;
    std::cout << "BoxTest 的体积：" << volume << std::endl;

    // box 2 的体积
    volume = Box2.height * Box2.length * Box2.breadth;
    std::cout << "Box2 的体积：" << volume << std::endl;


    // box 3 详述
    Box3.set(16.0, 8.0, 12.0);
    volume = Box3.get();
    std::cout << "Box3 的体积：" << volume << std::endl;
}

class Student {
public:
    Student() {
        std::cout << "默认构造函数" << std::endl;
    }

    //调用两个参数的构造函数
    //构造函数相互调用
    Student(int age) : Student(age, "dd") {
        std::cout << "一个参数构造函数" << std::endl;

        std::cout << age << name << std::endl;
    }

    // age(0) 相当于 this.age = 0;
    Student(int age, char *name) : age(0), name(name) {
        //动态分配
        this->name = (char *) malloc(sizeof(char) * 100);
        strcpy(this->name, name);

        this->age = age;
        // this->name = name;
    }

    //析构函数,如果有在对象内部开辟堆内存，没有参数
    ~Student() {
        //对象被回收的时候调用
        free(this->name);
        this->name = nullptr;
    }

    //拷贝构造函数
    Student(const Student &student) {
        std::cout << "拷贝构造函数" << std::endl;
       //  this->age = student.age;
       // this->name = student.name; //浅拷贝,指针指向同一片内存。

        //动态开辟内存一定要采用深拷贝，浅拷贝一旦释放源对象，拷贝的也会释放。
        this->name = (char *) malloc(sizeof(char) * 100);
        strcpy(this->name, student.name);
    }

private:
    int age;
    char *name;
    string className;
public:
    int getAge() const {
        return age;
    }

    void setAge(int age) {
        Student::age = age;
    }

    char *getName() const {
        return name;
    }

    void setName(char *name) {
        Student::name = name;
    }
};


void testClass2() {
    Student stu; //默认空参的构造函数
    stu.setAge(10);

    // 两个参数的构造函数
    // Student student(23, "asdf");

    //3 用new关键字，返回一个Student的一级指针
    Student *stu1 = new Student(23, "asdf");

    //调用这个方法，会调用析构函数
    delete (stu1);

    //4 用malloc,并没有调用空参的构造函数
    Student *stu2 = (Student *) malloc(sizeof(Student));
    stu.setAge(21);
    stu.setName("asdf");


}

// 3. malloc free new delete 都是开辟释放内存
void testClass3() {
    Student stu;//在栈上开辟内存

    Student *stu2 = (Student *) malloc(sizeof(Student));

    //malloc / free 是一套
    // malloc / free 不会调用构造函数和析构函数
    free(stu2);

    //3 用new关键字，返回一个Student的一级指针
    Student *stu1 = new Student(23, "asdf");

    //调用这个方法，会调用析构函数，用new 一定要用delete()
    delete (stu1);
}


void testClass4() {
    Student *stu = (Student *) malloc(sizeof(Student));

    //指针指向的地址，都指向同一个地址
    Student *stu2 = stu; //赋值，将所有里面的定义赋值，会调用拷贝构造函数

}

Student getStudent() {
    Student student(24); //栈开辟，方法执行完成，这个对象就会被回收，但是会调用拷贝构造函数。
    return student; //这里会返回一个新的Student对象，而栈内存开辟的stu会被回收
}


//stu 是该方法栈中一个新的对象，拷贝构造函数赋值
Student printStudent(Student stu) {
    std::cout << stu.getName() << stu.getAge() << std::endl;
}

void testClass5() {
    Student student(12, "test");

    const Student &student1 = student; //调用拷贝构造函数赋值

    Student stu2; //声明变量，开辟变量内存
    stu2 = student; //这个不会去调用拷贝构造函数，但是会赋值，和c类似

    //第二种场景，作为参数返回的时候会调用拷贝构造函数
    Student stu3 = getStudent();
    //printStudent(stu3); 这里调用会报错，因为调用了两次析构函数。



    //第三种场景，传递参数，会调用拷贝构造函数
    printStudent(student);

}