//对于参数中的函数调用顺序是未指定顺序的
#include <iostream>

void foo(int a, int b) {
    std::cout << a << "," << b << std::endl;
}

int get_num() {
    static int i = 0;
    return ++i;
}

int main() {
    foo(get_num(), get_num());      // 无序调用get_num()
}

/**
 */