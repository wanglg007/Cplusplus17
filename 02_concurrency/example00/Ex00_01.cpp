//(0.1)Hello World
#include <iostream>
#include <thread>           //(1)

void hello() {              //(2)
    std::cout << "Hello Concurrent World" << std::endl;
}

int main() {
    std::thread t(hello);   //(3)
    t.join();               //(4)
}

/**
 * (1)标准C++库中对多线程支持的声明在新的头文件中，用于管理线程的函数和类在<thread>中声明，而保护共享数据的函数和类在其他头文件中声明
 * (2)每个线程都必须具有一个初始函数，新线程的执行在这里开始
 * (3)被命名为t的std::thread对象拥有新函数hello作为其初始化函数
 * (4)导致调用线程(main)等待与std::thread对象相关联的线程
 */