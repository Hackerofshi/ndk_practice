//
// Created by shixin on 2022/11/1.
//

struct MyStruct {
    char *name;
    union {
        int x, r, s;
    };
};
typedef struct MyStruct Student;// 给 MyStruct 取一个别名叫做 Student


int teststudent1() {
    Student student = {};
    return 0;
}