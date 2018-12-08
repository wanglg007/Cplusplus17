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
 * (1)标准C++库中对多线程支持在新的头文件：管理线程的函数和类在<thread>中声明，而保护共享数据的函数和类在其他头文件中声明;
 * (2)因为每个线程都必须具有一个初始函数(initial function)，新线程的执行在这里开始。
 * (3)新的线程启动
 * (4)等待线程执行完毕后继续执行
 */