//
// Created by shixin on 2022/7/30.
//
#include <iostream>

using namespace std;

void teststr(){
    // C++ STL(Standard Template Library) 准模板库 ：容器 + 迭代器 + 算法
    // 1. 对象的构建
    string str1 = "123";
    string str2("123");
    string str3(5, 'A');// 5 个 A = AAAAA
    string *str4 = new string("123");

    // cout << str1.c_str() <<endl;
    // cout << str2.c_str() << endl;
    // cout << str3.c_str() << endl;
    // cout << str4->c_str() << endl;

    // string 与 char* 相互之间转换 c_str()
    // const char* c_str1 = str1.c_str();
    // cout << c_str1 << endl;

    // char* -> string
    char* c_str = "Darren"; //char *比较难操作
    string str(c_str);// 转成对象
    cout << str.c_str() << endl;
}

#define D_SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <algorithm>// STL 算法包
#include <cctype> // 用到一个函数指针，回调函数

using namespace std;

/*
void main(){

	string str("1234567");

	// 1. 字符串的遍历
	for (int i = 0; i < str.length(); i++)
	{
		cout << str[i] << endl;
	}

	// 迭代器遍历
	for (string::iterator it = str.begin(); it < str.end(); it++)
	{
		cout << *it << endl;
	}

	try{
		for (int i = 0; i < str.length()+2; i++)
		{
			cout << str.at(i) << endl;// 如果越界会抛异常
		}

		for (int i = 0; i < str.length()+2; i++)
		{
			cout << str[i] << endl;// 会导致程序宕机，AS里面是可以的
		}
	}
	catch (...){
		cout << "异常了" << endl;
	}


}


// 添加，删除，替换，查找，大小写转换
void main(){

	// 添加
	string str1 = "123";
	string str2 = "456";

	// str1 = str1 + str2;
	// str1.append(str2);


	cout << str1.c_str() << endl;

	getchar();
}


void main(){

	// 删除
	string str1 = "123 abc 123 abc 123";

	// str1.erase(0,3);// 第一个参数：从哪里开始 ； 第二个参数：删除几个（默认值，字符串的结尾）

	// 迭代器删除  2 bc 123 abc 123 解释
	for (string::iterator it = str1.begin(); it<str1.begin()+3; it++)// 删除一个字后都会从头开始计算
	{
		str1.erase(it);
	}

	cout << str1.c_str() << endl;

	getchar();
}


// 替换
void main(){

	string str1 = "123 abc 123 abc 123";
	// 第一个参数：从哪里开始
	// 第二个参数：替换几个
	// 第三个参数：替换成谁
	str1.replace(0,6,"1234");

	cout << str1.c_str() << endl;

	getchar();
}


// 查找
void main(){

	string str1 = "123 abc 123 abc 123";
	// 查找谁，从哪里开始
	// int position = str1.find("123",0);
	// 从后面往前面查
	int position = str1.rfind("123");

	cout << position << endl;

	getchar();
}


// 大小写转换
void main(){

	string str1 = "AAA abc BBB abc 123";
	// 转换成大写
	// transform(str1.begin(), str1.end(), str1.begin(), toupper);

	transform(str1.begin(), str1.end(), str1.begin(), tolower);

	cout << str1.c_str() << endl;

	getchar();
}


// 作业
// string str1 = "AAA abc BBB abc 123";  abc 全部替换成 def


void main(){
	// 寄存器上面的变量不能取地址，c编译器限制，register 可以快速
	register int a = 10;
	// c++ 很多默认情况下就会有 register 增强
	printf("%d", &a);

	for (register int i = 0; i < 10; i++)
	{

	}

	getchar();
}

// 参数处理增强
int a = 10;

void main(){

	printf("%d", a);

	getchar();
}


class Student
{
public:
	char* name;

public:
	void print() const{
		// name = "1234";
	}
};

// 2.1 使用场景
void main(){
	// 1. 修饰变量不可改变
	// const int number = 1;
	// 修饰指针 常量指针，指针常量
	int number = 1;
	// const int *n_p = &number;
	// *n_p = 2;
	// 指针常量
	// int * const n_p = &number;
	// n_p = 200;

	// 拷贝构造函数，运算符重载，方法后面（都是由于它的成员不可改变）

	cout << "Hello" << endl;

	getchar();
}

// 2.2 c / c++ 之间的区别
// c 里面 const 是一个伪命题，可以通过指针去操作，编译时期的检测
// c++ 里面不光在编译时期做处理，还在运行时做了处理
void main(){
	const int number = 12;
	// number = 23;
	int *number_p = const_cast<int*>(&number);// 计算通过 const_cast 转换已经不能修改值
	*number_p = 20;
	cout << number << endl;
	cout << *number_p << endl;

	getchar();
}
// 2.3 c / c++ 各自的实现原理
// 使用学习的时候不要过多的去纠结原理
// 遇到一些奇怪的现象，想办法去分析


// 3. 引用加强
// 3.1 引用的使用场景(回顾，好处)
// 3.2 引用的本质剖析

void change(int& number1,int& number2){
	int temp = number1;
	number1 = number2;
	number2 = temp;
}
// 其实引用本质就是 指针 。引用是一块内存的别名
void change(int& number1){// 相当于 change(int* number1)
	number1 = 12;// 相当于 *number1 = 12;
}

void main(){

	int number1 = 10;
	int number2 = 20;

	change(number1);

	cout << number1 << endl;

	getchar();
}


int get(){
	int number = 10;
	return number;
}

 //返回一个引用
int& get1(){
	int number = 10;
	return number;
}


// 引用做左值和右值的问题
void main(){

	int number1 = get(); // 10
	cout << number1 << endl;

	int number2 = get1(); // 10
	cout << number2 << endl;


	int& number3 = get1(); //? 有的编译器会是野指针 12478
	cout << number3 << endl;

	getchar();
}


class Sudent
{
private:
	string name;
public:
	Sudent(string name){
		this->name = name;
	}

public:
 	string getName(){ // Java 想都别想
		return this->name;
	}
   //此时是一个引用，可以当左值，又可以当右值
	string& getName1(){ // Java 不可以这样使用
		return this->name;
	}
};

void main(){

	Sudent stu = Sudent("Darren");

	// 改不动，右值
	// stu.getName() = "Jack";

	stu.getName1() = "Jack";

	cout << stu.getName().c_str() << endl;

	// vector front back

	getchar();
}
*/


/*
class Exception : public out_of_range
{
public:
    Exception(string msg) :out_of_range(msg){
        cout << "构造函数" << endl;
    }

    ~Exception(){
        cout << "析构函数" << endl;
    }
};

void c_method(){// 如果是自己的写 ，1000 多行
    cout << "抛异常" << endl;
    throw new Exception("异常了");
}

int main(){
    try{
        c_method();
    }
    catch (Exception* e){
        //重点
        // --Exception：多次构造函数和析构函数
        // --Exception& 避免了多次创建对象 (用的最多的)
        // --Exception* 创建的对象会被析构，如果使用局部函数或者成员就会是一个野指针
        cout << "try异常:"<< e->what() << endl;
        delete e;
    }
    catch (...){
        cout << "其他异常" << endl;
    }

    getchar();
}
*/


// 谓词（函数谓词） ：按找特定的规则所编写的函数谓词
//一个参数叫做一元
//bool compare(const Student& _Left, const Student& _Right){
//    return _Left.grade > _Right.grade;
//}
//
//// 函数对象 仿函数
//struct comparefuction
//{
//    // 函数重载了 () 运算符，函数对象，仿函数，只有这一个函数
//    bool operator()(const Student& _Left, const Student& _Right) const{
//        return _Left.grade > _Right.grade;
//    }
//};

class Compare{
public:
    void operator()(){
      cout << ""<<endl;
    }
};

void teststr1(){
    Compare compare;

    //和函数的调用基本一致
    compare();
}

/*
  #include<algorithm> // 预定义函数 ，已经实现好的一些算法头文件


// 仿函数 - 一元谓词 （能够记录状态）
class PrintObj
{
public:
	int count = 0; //记住执行多少次的状态
public:
	void operator()(int number){
		cout << number << endl;
		count++;
	}
};


// 回调函数和仿函数的区别
 //仿函数能记录状态，如执行次数
 //回调函数不行
void main() {

    set<int> set1;
    set1.insert(1);
    set1.insert(2);
    set1.insert(3);
    set1.insert(4);

    // for_each 迭代器 ,非常重要的一点就是：仿函数如果要保存记录状态，要确保对象一致，可以用返回值
    // for_each(set1.begin(),set1.end(),print);
    PrintObj printObj;

    //返回一个仿函数对象，如果要获取到仿函数的状态，就需要拿到返回值
    printObj = for_each(set1.begin(), set1.end(), printObj); //作为一个参数传入
    cout << "个数：" << printObj.count << endl;

    getchar();
}


class CompareObj
{
public:
    int count = 0;
public:
    bool operator()(const string str1, const string str2){
        return str1 < str2;
    }
};

void main(){
    // 二元谓词的仿函数，这里面传入的仿函数就可以去排序
    set<string, CompareObj> set1;
    set1.insert("aaa");
    set1.insert("aAa");
    set1.insert("ccc");
    set1.insert("ddd");
    // 是否包含 aaa , 遍历比较 ， 找方法
    for (set<string>::iterator it = set1.begin(); it != set1.end(); it++)
    {
        cout << (*it).c_str() << endl;
    }
    getchar();
}
*/



// 1，种方式自定义仿函数（函数对象）
/*
class Equal
{
private:
    int equal_number;
public:
    Equal(int equal_number){
        this->equal_number = equal_number;
    }
public:
    bool operator()(const int& number){
        return number == equal_number;
    }
};

void main(){
    vector<int> vector1;
    vector1.push_back(1);
    vector1.push_back(2);
    vector1.push_back(3);
    vector1.push_back(2);
    vector1.push_back(4);
    vector1.push_back(2);

    // 找集合中 等于 2 的个数
    int count = count_if(vector1.begin(), vector1.end(), Equal(2));
    cout << "count = " << count << endl;

    // 预定义好的函数对象 + 函数适配器
    count = count_if(vector1.begin(), vector1.end(), bind2nd(equal_to<int>(),2));
    cout << "count = " << count << endl;

    getchar();
}

 void main(){
	// java 中把对象添加到了集合
	// c++ 中会调用对象的拷贝构造函数，存进去的是另一个对象
	// 第一个错误：没有默认的构造函数
	// 第二个错误：析构函数也可能回调用多次，如果说在析构函数中释放内存，需要在拷贝构造函数中进行深拷贝
	vector<Person> vector1;

	Person person("Darren");
	vector1.push_back(person);

	person.setName("Jack");

	Person person1 = vector1.front();

	cout << person1.name.c_str() << endl;

	getchar();
}

*/


