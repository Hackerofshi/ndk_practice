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