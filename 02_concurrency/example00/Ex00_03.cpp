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
 * 若需要等待线程，则std::tread实例需要使用join()函数。将my_thread.detach()替换为my_thread.join()可以确保局部变量在线程完成后才被销毁。
 *
 * join()是简单粗暴的等待线程完成或不等待。当需要对等待中的线程有更灵活的控制时，比如看某个线程是否结束，或者只等待一段时间。需要使用
 * 其他机制来完成，比如条件变量和期待(futures)。调用join()的行为还清理了线程相关的存储部分，这样std::thread对象将不再与已经完成的线程
 * 有任何关联。这意味着只能对一个线程使用一次join();一旦已经使用过join()，std::thread 对象就不能再次加入，当对其使用joinable()时，将返回否。
 *
 * 若需要对一个还未销毁的std::thread对象使用join()或detach()。若想要分离一个线程，则可以在线程启动后，直接使用detach()进行分离。如果打算
 * 等待对应线程，则需要细心挑选调用join()的位置。当在线程运行之后产生异常，在join()调用之前抛出，就意味着很这次调用会被跳过。
 *
 * 避免应用被抛出的异常所终止就需要作出一个决定。通常当倾向于在无异常的情况下使用join()时，需要在异常处理过程中调用join()，从而避免生命周期的问题。
 *
 * 代码使用了try/catch块确保访问本地状态的线程退出后函数才结束。当函数正常退出时会执行到②处；当函数执行过程中抛出异常，程序会执行到①处。
 * try/catch块能轻易的捕获轻量级错误。如需确保线程在函数之前结束，再确定一下程序可能会退出的途径，可以提供一个简洁的机制(RAII)来做解决该问题。
 */