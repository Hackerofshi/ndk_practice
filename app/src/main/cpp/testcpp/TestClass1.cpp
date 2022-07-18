//
// Created by shixin on 2022/7/18.
//

#include <iostream>

using namespace std;


class Line {
    // int a; //在类里面不写是什么类型，默认是 private 的。
public:
    double length;

    void setLength1(double len);

    double getLength1();
};

// 成员函数定义
double Line::getLength1() {
    return length;
}

void Line::setLength1(double len) {
    length = len;
}


//protected（受保护）成员
//protected（受保护）成员变量或函数与私有成员十分相似，但有一点不同，protected（受保护）成员在派生类（即子类）中是可访问的。
class Box {
protected:
    double width;
};

class SmallBox : Box // SmallBox 是派生类
{
public:
    void setSmallWidth(double wid);

    double getSmallWidth(void);
};

// 子类的成员函数
double SmallBox::getSmallWidth(void) {
    return width;
}

void SmallBox::setSmallWidth(double wid) {
    width = wid;
}

class A {
public:
    int a;

    A() {
        a1 = 1;
        a2 = 2;
        a3 = 3;
        a = 4;
    }

    void fun() {
        cout << a << endl;    //正确
        cout << a1 << endl;   //正确
        cout << a2 << endl;   //正确
        cout << a3 << endl;   //正确
    }

public:
    int a1;
protected:
    int a2;
private:
    int a3;
};

class B : public A {
public:
    int a;

    explicit B(int i) {
        A();
        a = i;
    }

    void fun() {
        cout << a << endl;       //正确，public成员
        cout << a1 << endl;       //正确，基类的public成员，在派生类中仍是public成员。
        cout << a2 << endl;       //正确，基类的protected成员，在派生类中仍是protected可以被派生类访问。
        // cout << a3 << endl;       //错误，基类的private成员不能被派生类访问。
    }
};

// 程序的主函数
int main0() {
    Line line;

    // 设置长度
    line.setLength1(6.0);
    cout << "Length of line : " << line.getLength1() << endl;

    // 不使用成员函数设置长度
    line.length = 10.0; // OK: 因为 length 是公有的
    cout << "Length of line : " << line.length << endl;
    return 0;
}


//explicit 避免隐式调用
class Point {
public:
    int x, y;

    explicit Point(int x = 0, int y = 0)
            : x(x), y(y) {}
};

void displayPoint(const Point &p) {
    cout << "(" << p.x << ","
         << p.y << ")" << endl;
}

void main1() {
    displayPoint(Point(1));
    Point p(1);

    //隐式调用，不加explicit关键字时
    //    displayPoint(1);
    //    Point p = 1;
}