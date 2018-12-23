//(0.3)等待线程完成
#include <thread>

void do_something(int &i) {
    ++i;
}

struct func {
    int &i;

    func(int &i_) : i(i_) {}

    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_something(i);
        }
    }
};

void do_something_in_current_thread() {

}

void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    try {
        do_something_in_current_thread();
    } catch (...) {
        t.join();           //(1)
        throw;
    }
    t.join();               //(2)
}

int main() {
    f();
}

/**
 * 代码使用了try/catch块确保访问局部状态的线程退出后函数前结束。无论函数是正常退出①还是异常②中断。使用try/catch不是一个理想的方案。
 * try/catch块能轻易的捕获轻量级错误。如需确保线程在函数之前结束，再确定一下程序可能会退出的途径，可以提供一个简洁的机制(RAII)来做解
 * 决该问题。
 */


















