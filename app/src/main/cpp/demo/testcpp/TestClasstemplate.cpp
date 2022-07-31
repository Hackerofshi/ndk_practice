//
// Created by shixin on 2022/7/27.
//
using namespace std;

//其他类型用的是模板函数
template<typename T>
T add(T number1, T number2) {

    cout << "模板函数被调用" << endl;
    return number1 + number2;
}

// 当普通函数和模板函数同时存在的时候，优先会调用普通函数
//普通函数
int add(int number1, int number2) {
    cout << "int add 被调用" << endl;
    return number1 + number2;
}



// so 是很难被反编译，比java安全为啥？
// java small 语法,反编译阿


//模板函数编译时候生成的方式
// 打算生成 3 个方法 ， 发现有一个 int 函数就不会额外生成，
// 发现 double float 没有，就会生成两个

// 1-2 是汇编代码     xposed 调试 ，反编译so就是解释解释汇编
// 1. 编译的时候 c 和 c++ 编译器会对 cpp/c 文件做一些优化（模板函数）
// 2. java 有 small -> class，也会生成 c/c++ 编译器能够认识的一套代码
// public:void _thiscall Activity::initView(void)

// 3. 生成二进制文件 .obj .o 等等一些文件，链接其他模板 cmake 中一些链接
// 4. 最后就会可执行文件 .dll, .so , .exe

void main() {

    int sum1 = add(1, 2);

    double sum2 = add(1.0, 2.3);

    float sum3 = add(1.0f, 2.4f);


    cout << sum1 << " , " << sum2 << endl;

    getchar();
}

//模板类，语法和模板函数很像
template<typename T>
class Callback {
public:
    void onError() {

    }

    void onSucceed(T result) {
        cout << result << endl;
    }
};

// 模板类继承 ，子类如果也是模板类
template<typename T>
class HttpCallback1 : public Callback<T> {

};

// 如果子类不是模板类
class HttpCallback : public Callback<int> {

};

void testtemplateClass() {
    HttpCallback<int> *callback = new HttpCallback<int>();
    HttpCallback1<int> *callback = new HttpCallback1<int>();

    callback->onSucceed(12);

    getchar();
}