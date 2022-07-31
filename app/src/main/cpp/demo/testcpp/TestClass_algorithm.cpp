//
// Created by shixin on 2022/7/31.
//

#define _SCL_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <algorithm>

using namespace std;

/*
// 自定义重载 () 运算符
// 1. 预定义函数对象和函数适配器
void main(){
	// c/c++ 提供了很多定义好的函数对象
	// 常见的几个 less ，greater，plus，equal_to
	plus<string> strAdd;
	string str = strAdd("aaa","bbb");

	// cout << str.c_str() << endl;

	set<string, greater<string>> set1;
	set1.insert("aaa");
	set1.insert("bbb");
	set1.insert("ccc");

	// 判断是不是包含 aaa
	// 怎么写仿函数，一定要确定好你的仿函数的参数
	template<class _Ty = void>
	struct multiplies
		: public binary_function<_Ty, _Ty, _Ty>
	{	// functor for operator*
		_Ty operator()(const _Ty& _Left, const _Ty& _Right) const
		{	// apply operator* to operands
			return (_Left * _Right);
		}
	}

 	// bind2nd 函数适配器 , aaa 相当于 equal_to 中的 right
 	//把两次参数进行绑定
	set<string, greater<string>>::iterator find_it = find_if(set1.begin(), set1.end(),bind2nd(equal_to<string>(),"aaa"));
	if (find_it != set1.end()){
		cout << "找到了" << (*find_it).c_str() << endl;
	}
	else
	{
		cout << "没有找到" << endl;
	}

	getchar();
}


// 1，种方式自定义仿函数（函数对象）
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


void print(int number){
	cout << number << endl;
}

// 进行修改
int transform_print(int number){
	// cout << number << endl;
	return number + 3;
}

// foreach，transform，find，find_if，count，count_if，megre，sort，random_shuffle，copy，replace
// 常用预定义算法 循环，增，删，改，查
void main(){
	vector<int> vector1;
	vector1.push_back(1);
	vector1.push_back(2);
	vector1.push_back(3);
	vector1.push_back(4);

	// for_each(vector1.begin(), vector1.end(),print);
	vector<int> vector2;
	vector2.resize(vector1.size());

	transform(vector1.begin(), vector1.end(), vector2.begin(), transform_print);

	for_each(vector2.begin(), vector2.end(), print);

	getchar();
}


// find，find_if
void main(){
	vector<int> vector1;
	vector1.push_back(1);
	vector1.push_back(2);
	vector1.push_back(3);
	vector1.push_back(4);

	vector<int>::iterator find_it = find(vector1.begin(), vector1.end(), 2);

	if (find_it != vector1.end()){
		cout << "包含" << endl;
	}
	else
	{
		cout << "不包含" << endl;
	}

	// 有没有大于2的，自定义函数对象，预定义函数对象+函数适配器，省略...

	getchar();
}


// count，count_if
void main(){
	vector<int> vector1;
	vector1.push_back(1);
	vector1.push_back(2);
	vector1.push_back(3);
	vector1.push_back(2);
	vector1.push_back(4);

	int number = count(vector1.begin(), vector1.end(), 2);

	cout << "等于2的个数:" << number << endl;

	number = count_if(vector1.begin(), vector1.end(), bind2nd(less<int>(), 2));

	cout << "小于2的个数:" << number << endl;

	number = count_if(vector1.begin(), vector1.end(), bind2nd(greater<int>(), 2));

	cout << "大于2的个数:" << number << endl;

	getchar();
}

class _merge
{
public:
	bool operator()(int number1,int number2){
		return true;
	}

};

void print(int number){
	cout << number << endl;
}

// megre，sort，random_shuffle，copy，replace
void main(){

	// 两个有序数组进行合并 - 归并排序
	vector<int> vector1;
	vector1.push_back(1);
	vector1.push_back(2);
	vector1.push_back(3);

	vector<int> vector2;
	vector1.push_back(4);
	vector1.push_back(5);
	vector1.push_back(6);

	vector<int> vector3;
	vector3.resize(6);
	merge(vector1.begin(), vector1.end(), vector2.begin(), vector2.end(), vector3.begin());
	for_each(vector3.begin(), vector3.end(), print);

	getchar();
}


void print(int number){
	cout << number << endl;
}

void main(){

	vector<int> vector1;
	vector1.push_back(1);
	vector1.push_back(3);
	vector1.push_back(2);
	vector1.push_back(4);

	sort(vector1.begin(),vector1.end(),less<int>());
	for_each(vector1.begin(), vector1.end(), print);

	cout << "循环结束" << endl;

	// 打乱循序
	random_shuffle(vector1.begin(), vector1.end());
	for_each(vector1.begin(), vector1.end(), print);

	getchar();
}


void print(int number){
	cout << number << endl;
}

// copy，replace
void main(){

	vector<int> vector1;
	vector1.push_back(1);
	vector1.push_back(2);
	vector1.push_back(3);
	vector1.push_back(4);

	vector<int> vector2;
	vector2.resize(2);
	copy(vector1.begin(), vector1.begin() + 2, vector2.begin());
	// for_each(vector2.begin(), vector2.end(), print);

	replace(vector1.begin(), vector1.end(), 2, 22);
	for_each(vector1.begin(), vector1.end(), print);

	getchar();
}

// 头部的封装 ToolBar 思考：自定义 View  4中样式
// 思考问题 ，方式去解决 NavigationBar (各种源码各种框架)

// 过度设计 ：扩展性不高实用性不强 ，继承(代码)过于复杂，花哨

// 模板类 _InIt 名字 , _First, _Last , 返回值 是一个类型
// _Pr _Pred 这是什么？
template<class _InIt,
class _Pr> inline
	_InIt find_if(_InIt _First, _InIt _Last, _Pr _Pred)
{	// find first satisfying _Pred
		_DEBUG_RANGE(_First, _Last);
		_DEBUG_POINTER(_Pred);
		return (_Rechecked(_First,
			_Find_if(_Unchecked(_First), _Unchecked(_Last), _Pred)));
	}
template<class _InIt,
class _Pr> inline
	_InIt _Find_if(_InIt _First, _InIt _Last, _Pr _Pred)
{	// find first satisfying _Pred
		for (; _First != _Last; ++_First)
		// if 返回值是一个 bool 类型 ，往 _Pred 里面传递了一个参数 ，要么就是回调函数要么就是仿函数
		if (_Pred(*_First))
			break;
		return (_First);
}
*/