//
// Created by shixin on 2022/7/18.
//

//类的友元函数是定义在类外部，但有权访问类的所有私有（private）成员和保护（protected）成员。尽管友元函数的原型有在类的定义中出现过，但是友元函数并不是成员函数。
//
//友元可以是一个函数，该函数被称为友元函数；友元也可以是一个类，该类被称为友元类，在这种情况下，整个类及其所有成员都是友元。
//
//如果要声明函数为一个类的友元，需要在类定义中该函数原型前使用关键字 friend，如下所示：

#include <iostream>

using namespace std;

//友元函数没有 this 指针，因为友元不是类的成员。只有成员函数才有 this 指针。
class Box {
    double width;
public:
    friend void printWidth(Box box);

    friend class BigBox;

    void setWidth(double wid);
};


//友员类
class BigBox {
public:
    void Print(int width, Box &box) {
        // BigBox是Box的友元类，它可以直接访问Box类的任何成员
        box.setWidth(width);
        cout << "Width of box : " << box.width << endl;
    }
};

// 成员函数定义
//在 C++ 中，每一个对象都能通过 this 指针来访问自己的地址。
// this 指针是所有成员函数的隐含参数。因此，在成员函数内部，它可以用来指向调用对象。
void Box::setWidth(double wid) {
    width = wid;
}

// 请注意：printWidth() 不是任何类的成员函数
void printWidth(Box box) {
    /* 因为 printWidth() 是 Box 的友元，它可以直接访问该类的任何成员 */
    cout << "Width of box : " << box.width << endl;
}


// 程序的主函数
int main4() {
    Box box;
    BigBox big;


    // 使用成员函数设置宽度
    box.setWidth(10.0);

    // 使用友元函数输出宽度
    printWidth(box);

    // 使用友元类中的方法设置宽度
    big.Print(20, box);

    return 0;
}


class BoxTest {
public:
    static int objectCount;

    // 构造函数定义
    BoxTest(double l = 2.0, double b = 2.0, double h = 2.0) {
        cout << "Constructor called." << endl;
        length = l;
        breadth = b;
        height = h;
        // 每次创建对象时增加 1
        objectCount++;
    }

    double Volume() {
        return length * breadth * height;
    }

    static int getCount() {
        return objectCount;
    }

private:
    double length;     // 长度
    double breadth;    // 宽度
    double height;     // 高度
};

// 初始化类 Box 的静态成员
int BoxTest::objectCount = 0;

void main3() {

    // 在创建对象之前输出对象的总数
    cout << "Inital Stage Count: " << BoxTest::getCount() << endl;

    BoxTest Box1(3.3, 1.2, 1.5);    // 声明 box1
    BoxTest Box2(8.5, 6.0, 2.0);    // 声明 box2

    // 在创建对象之后输出对象的总数
    cout << "Final Stage Count: " << BoxTest::getCount() << endl;

}

//命名空间

// 第一个命名空间
namespace first_space {
    void func() {
        cout << "Inside first_space" << endl;
    }
}
// 第二个命名空间
namespace second_space {
    void func() {
        cout << "Inside second_space" << endl;
    }
}

int main8() {

    // 调用第一个命名空间中的函数
    first_space::func();

    // 调用第二个命名空间中的函数
    second_space::func();

    return 0;
}


class Person {
protected:
    char *name;
    int age;
public:
    Person(char *name, int age) {
        this->name = name;
        this->age = age;
    }

    ~Person(){
        cout << "Person 的析构函数" << endl;
    }
};

class Course {
private:// java String
    string name;
public:
    Course(string name) {
        this->name = name;
    }

public:
    string _name() {
        return this->name;
    }
};

class Student : public Person {
private:
// char* courseName;
    Course course; //属性
public:
    Student(char *name, int age, string courseName) : Person(name, age),    // 初始化父类的属性
            //属性的初始化
                                                      course(courseName) {

        // this->courseName = courseName;
        // 字符串转成指针打印
        //cout << name << "," << age << "," << course._name().c_str() << endl;

    }
    ~Student(){
        cout << "Student 的析构函数" << endl;
    }

}

// 构造函数和析构函数
void constructor(){
    // 构造函数：先父类 -> 再子类
    // 析构函数：先子类 -> 再父类
    Student *stu = new Student("Darren",24);
    delete stu;

}



class A {
public:
    char *name;
};

class B : A {
    int name; //这样也会有二义性
};

class C : A {

};


//这里就会出现二义性
class D : B, C {

};

//所以要用虚继承
// 2.2 虚继承 （二义性）
//class A{
//public:
//    char* name;
//};
//
//class B : virtual public A{ // 确保继承过来的相同属性或者函数，只存在一份拷贝
//
//};
//
//class C :virtual public A{
//
//};
//
//class D : public B ,public C
//{
//
//};

class Activity{
public:
    // 支持多态，虚函数,加上关键字才支持多态
    virtual void onCreate(){
        cout << "Activity 中的 onCreate" << endl;
    }
};

class MainActivity : public Activity
{
public:
    void onCreate(){
        cout << "MainActivity 中的 onCreate" << endl;
    }
};

class WelcomeActivity : public Activity
{
public:
    void onCreate(){
        cout << "WelcomeActivity 中的 onCreate" << endl;
    }
};

//静态多态，编译时就确定
int add(int number1, int number2){
    return number1 + number2;
}

double add(double number1, double number2){
    return number1 + number2;
}

float add(float number1, float number2){
    return number1 + number2;
}

void testactivity(){
    Activity *activity1 = new MainActivity();// 父类 = new 子类对象
    Activity *activity2 = new WelcomeActivity();

    // activity->onCreate();
    // c++ 中的多态是怎样的，默认情况下不存在
    // 父类指向子类的引用，重写 ，里氏替换原则
    // 程序在编译期间并不知晓运行的状态（我需要运行那个函数），
    //只要在真正运行的过程中才会去找需要运行的方法
    // 解释一下 java 中的多态 20K
    startActivity(activity1);
    startActivity(activity2);

    // c++ 多态：动态多态(子父类)，
    //静态多态（函数的重载）（编译过程确定性的区别）
    add(1,2);
    add(0.0,0.0);

}

// java 中类似的 抽象类，接口 纯虚函数
class BaseActivity {
public:
    void OnCreate(){

    }
    // 子类必须要实现
    virtual void initData() = 0;// 虚函数，没有实现的，类似于 java 中的抽象方法，如果子类不实现会报错

    virtual void initView() = 0;

};
// 如果不实现父类的纯虚函数，
// 那么 MainActivity 也会变成抽象类，抽象类不允许实例化
class MainActivity : public BaseActivity
{
public:
    void initData(){
        cout << "initData" << endl;
    }

    void initView(){
        cout << "initView" << endl;
    }
};

void testVirtual1(){

    BaseActivity *m_a = new MainActivity();

    m_a->initView();
    m_a->onCreate();
}


// 所有的函数都是虚函数，那么就可以认为是接口
class ClickListener {
public:
    virtual void click() = 0;
};

//子类必须实现
class ImageClicklistener:public ClickListener{
public :
    void click(){
        cout << "图片点击" << endl;
    }
};


void click(ClickListener *listener){
    listener->click();
}


// 函数指针作为参数传递  返回值(函数名)(参数),
// 相当于一个回调函数
void click(void(*c)){
    // 压缩开始
    c();// 输出压缩进度
    // 压缩结束
}

void testClick(){
    // 函数指针的时候：回调可以用 指针函数作为回调，纯虚函数类进行回调（接口）
    // ClickListener *listener = new ImageClickListener();

    // listener->click();
    // click(listener);

    // 自己再去了解加深一下
    click(click);


    getchar();
}

//模板函数  java中的泛型

template <typename T>// 模板函数的定义
T add(T number1,T number2{
    return number1 + number2;
}


void tesTemplate(){

    int sum1 = add(1,2);

    cout << sum1 << endl;

    int sum2 = add(1.0, 2.0);

    cout << sum2 << endl;

    int sum3 = add(1.0f, 2.0f);

    cout << sum3 << endl;
}
