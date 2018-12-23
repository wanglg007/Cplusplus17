//(0.2)当线程仍然访问局部变量时返回的函数
#include <thread>

void do_something(int &i) {
    ++i;
}

struct func {
    int &i;

    func(int &i_) : i(i_) {
    }

    void operator()() {
        for (unsigned j = 0; j < 100000; ++j) {
            do_something(i);                //(1)对悬空引用可能的访问
        }
    }
};

void oops() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();                     //(2)不等待线程结束
    //(3)新线程可能还在运行
}

int main() {
    oops();
}

/**
 * 当oops退出③时与my_thread相关联的新线程可能仍然在运行，因为通过调用detach()②已经显式的决定不等待它。如果线程仍在运行，则
 * 在下次调用do_something(i)①时就会访问一个已被销毁的变量。这就像普通的单线程代码那样，允许对局部变量的指针或引用持续到函数
 * 退出之后绝不是一个好主意。但对于多线程代码更容易犯这样的错误，因为当它发生的时候，并不一定是显而易见的。
 *
 * 一个常见的处理这种情况的方式是线程函数自包含，并且把数据复制到该线程中而不是共享数据。如果为线程函数使用一个可调用对象，该
 * 对象本身被复制到该线程中，那么原始对象就可以立即被销毁。但是仍然需要警惕包含有指针或引用的对象。特别的，在一个访问局部变量
 * 的函数中创建线程是个糟糕的主意，除非能保证线程在函数退出前完成。
 * 另外，通过结合线程可以确保在函数退出前，该线程执行完毕。
 *
 */
