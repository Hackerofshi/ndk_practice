//
// Created by shixin on 2022/7/18.
//

#include <iostream>

using namespace std;

//类的析构函数
//类的析构函数是类的一种特殊的成员函数，它会在每次删除所创建的对象时执行。
//析构函数的名称与类的名称是完全相同的，只是在前面加了个波浪号（~）作为前缀，
// 它不会返回任何值，也不能带有任何参数。析构函数有助于在跳出程序（比如关闭文件、释放内存等）前释放资源。
class Line {
public:
    void setLength(double len);

    double getLength(void);

    Line();   // 这是构造函数声明
    ~Line();  // 这是析构函数声明

private:
    double length;
};

// 成员函数定义，包括构造函数
Line::Line(void) {
    cout << "Object is being created" << endl;
}

Line::~Line(void) {
    cout << "Object is being deleted" << endl;
}

void Line::setLength(double len) {
    length = len;
}

double Line::getLength(void) {
    return length;
}

// 程序的主函数
int main() {
    Line line;

    // 设置长度
    line.setLength(6.0);
    cout << "Length of line : " << line.getLength() << endl;

    return 0;
}


//拷贝构造函数
//拷贝构造函数是一种特殊的构造函数，它在创建对象时，是使用同一类中之前创建的对象来初始化新创建的对象。拷贝构造函数通常用于：
//1.通过使用另一个同类型的对象来初始化新创建的对象。
//2.复制对象把它作为参数传递给函数。
//3.复制对象，并从函数返回这个对象。

class LineRect {
public:
    int getLength(void);

    LineRect(int len);//简单构造函数
    LineRect(const LineRect &obj) //拷贝构造函数
    ~LineRect();// 析构函数
private:
    int *ptr;
};


// 成员函数定义，包括构造函数
LineRect::LineRect(int len) {
    cout << "调用构造函数" << endl;
    ptr = new int;
    *ptr = len;
}

LineRect::LineRect(const LineRect &obj) {
    cout << "调用拷贝构造函数并为指针 ptr 分配内存" << endl;
    ptr = new int;
    *ptr = *obj.ptr; // 拷贝值
}

LineRect::~LineRect(void) {
    cout << "释放内存" << endl;
    delete ptr;
}

int LineRect::getLength() {
    return *ptr;
}

void display(LineRect obj)
{
    cout << "line 大小 : " << obj.getLength() <<endl;
}



// 程序的主函数
int main1( )
{
    LineRect lineRect(10);

    display(lineRect);
    //调用构造函数
    //调用拷贝构造函数并为指针 ptr 分配内存
    //line 大小 : 10
    //释放内存
    //释放内存



    LineRect line1(10);

    LineRect line2 = line1; // 这里也调用了拷贝构造函数

    display(line1);
    display(line2);

    //调用构造函数
    //调用拷贝构造函数并为指针 ptr 分配内存

    //调用拷贝构造函数并为指针 ptr 分配内存
    //line 大小 : 10
    //释放内存

    //调用拷贝构造函数并为指针 ptr 分配内存
    //line 大小 : 10
    //释放内存


    //释放内存
    //释放内存


    return 0;
}
























