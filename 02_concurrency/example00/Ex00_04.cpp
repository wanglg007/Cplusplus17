//(0.4)使用RAII等待线程完成
#include <thread>

class thread_guard {
    std::thread &t;
public:
    explicit thread_guard(std::thread &t_) : t(t_) {}

    ~thread_guard() {
        if (t.joinable()) {     //(1)
            t.join();           //(2)
        }
    }

    thread_guard(thread_guard const &) = delete;                //(3)

    thread_guard &operator=(thread_guard const &) = delete;
};

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
    int some_local_state;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard g(t);

    do_something_in_current_thread();
}                           //(4)

int main() {
    f();
}

/**
 * 当线程执行到④处时，局部对象会按照构造函数的逆序被销毁。因此thread_guard对象g首先被销毁，并且析构函数②中线程被结合。即
 * 使do_something_in_current_thread引发异常而退出的情况下也会发生。
 *
 * 析构函数在调用join()②前首先测试thread_guard的析构函数是不是joinable()①的。因为对于一个给定的执行线程join()只能被调用
 * 一次，所以如果线程已经被结合，这样做是错误的。
 *
 * 拷贝构造函数和拷贝赋值操作被标记为=delete ③是为了不让编译器自动生成它们。直接对一个对象进行拷贝或赋值是危险的，因为这
 * 可能会弄丢已经加入的线程。通过删除声明，任何尝试给thread_guard对象赋值的操作都会引发一个编译错误。
 *
 * 如果不想等待线程结束可以分离(detaching)线程，从而避免异常安全(exception-safety)问题。不过就打破了线程与std::thread对象
 * 的联系，即使线程仍然在后台运行，分离操作也能确保std::terminate()在std::thread对象销毁才被调用。
 *
 */
















