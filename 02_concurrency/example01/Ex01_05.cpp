//(1.5)扩充(线程安全)堆栈
#include <exception>
#include <stack>
#include <mutex>
#include <memory>

struct empty_stack : std::exception {
    const char *what() const throw() {
        return "empty stack";
    }
};

template<typename T>
class threadsafe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack() {}

    threadsafe_stack(const threadsafe_stack &other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.data;                  // 1 在构造函数体中的执行拷贝
    }

    threadsafe_stack &operator=(const threadsafe_stack &) = delete;

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(new_value);
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
            throw empty_stack();            // 在调用pop前，检查栈是否为空
        std::shared_ptr<T> const  res(std::make_shared<T>(data.top()));
        data.pop();
        return res;
    }

    void pop(T &value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())
            throw empty_stack();
        value = data.top();
        data.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

int main() {
    threadsafe_stack<int> si;
    si.push(5);
    si.pop();
    if (!si.empty()) {
        int x;
        si.pop(x);
    }
}

/**
 * 堆栈可以拷贝:拷贝构造函数对互斥量上锁，再拷贝堆栈。构造函数体中①的拷贝使用互斥量来确保复制结果的正确性，这样的方式比成员初始化列表好。
 *
 * 之前对top()和pop()函数的讨论中，恶性条件竞争已经出现，因为锁的粒度太小，需要保护的操作并未全覆盖到。不过，锁住的颗粒过大同样会有问题。一个全局互斥量
 * 要去保护全部共享数据，在一个系统中存在有大量的共享数据时，因为线程可以强制运行，甚至可以访问不同位置的数据，抵消了并发带来的性能提升。
 */