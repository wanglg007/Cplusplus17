//(1.2)无意中传递了保护数据的引用
#include <mutex>

class some_data {
    int a;
    std::string b;
public:
    void do_something() {

    }
};

class data_wrapper {
private:
    some_data data;
    std::mutex m;
public:
    template<typename Function>
    void process_data(Function func) {
        std::lock_guard<std::mutex> l(m);
        func(data);                             // 1 传递“保护”数据给用户函数
    }
};

some_data *unprotected;

void malicious_function(some_data &protected_data) {
    unprotected = &protected_data;
}

data_wrapper x;

void foo() {
    x.process_data(malicious_function);         // 2 传递一个恶意函数
    unprotected->do_something();                // 3 在无保护的情况下访问保护数据
}

int main() {
    foo();
}

/**
 * 案例process_data看起来没有问题，std::lock_guard对数据做了很好的保护，但调用函数func①意味着foo能够绕过保护机制将函数malicious_function传递进
 * ②，故在没有锁定互斥量的情况下调用do_something()。
 *
 * 该段代码的问题在于根本没有做到保护：只是将所有可访问的数据结构代码标记为互斥。函数foo()中调用unprotected->do_something()的代码未能被标记为互斥。该情况
 * 下，只能由程序员来使用正确的互斥锁来保护数据。从乐观的角度上看：切勿将受保护数据的指针或引用传递到互斥锁作用域之外。无论是函数返回值，还是存储在外部可
 * 见内存，亦或是以参数的形式传递到用户提供的函数中去。
 *
 */