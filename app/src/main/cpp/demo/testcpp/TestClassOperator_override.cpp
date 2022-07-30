//
// Created by shixin on 2022/7/27.
//
using namespace std;

class Vector {
public:
    Vector(int x, int y) {
        this->x = x;
        this->y = y;
    }

    Vector(const Vector &vector) {
        this->x = vector.x;
        this->y = vector.y;
        cout << "拷贝构造函数" << endl;
    }

private:
    int x;
    int y;

public:
    int getX() const {
        return x;
    }

    void setX(int x) {
        Vector::x = x;
    }

    int getY() const {
        return y;
    }

    void setY(int y) {
        Vector::y = y;
    }

    //重载减号运算符
    //为什么要用引用？为了防止重复创建对象
    // const 关键常量，为了防止去修改值。
    Vector operator-(const Vector &vector) {
        int x = this.x - vector.x;
        int y = this->y - vector.y;

        Vector res(x, y)

        return res; //不建议返回引用
    }

    //自增运算符
    void operator++() {
        this.x = this->x++;
        this.y = this->y++;
    }

    void operator++(int) {// X++
        this->x = this->x++;
        this->y = this->y++;
    }

    // 输出运算符
    friend ostream & operator << (ostream &_Ostr, const Vector &vector){
        _Ostr << vector.x << "," << vector.y << endl;
        return _Ostr;
    }

    // 条件运算符
    bool operator == (const Vector &vector){
        return (this->x == vector.x && this->y == vector.y);
    }


};

// 定义在类的外面，一般来讲我们定义在类的里面
// 重载运算 + ：operator +
Vector operator + (Vector vector1, const Vector vector2){
    int x = vector1.getX() + vector2.getX();
    int y = vector1.getY() + vector2.getY();
    Vector vector(x,y);
    return vector;
}

void testOperator(){
    Vector vector1(2, 3);
    Vector vector2(2, 3);



    // java 中 string + string

    // char* str = "123" + "456";

    // 重载运算符 +
    // Vector vector = vector1 - vector2;

    // Vector vector(1, 2);
    // vector++;
    // ++vector;

    // cout << vector.getX() << " , " << vector.getY() << endl;
    // cout << vector << vector;
    bool isEqual = vector1 == vector2;
    cout << isEqual << endl;
}