//使用 std::memroy_order_consume 同步数据
#include <string>
#include <thread>
#include <atomic>
#include <assert.h>

struct X {
    int i;
    std::string s;
};

std::atomic<X *> p;
std::atomic<int> a;

void create_x() {
    X *x = new X;
    x->i = 42;
    x->s = "hello";
    a.store(99, std::memory_order_relaxed);     // 1
    p.store(x, std::memory_order_release);      // 2
}

void use_x() {
    X *x;
    while (!(x = p.load(std::memory_order_consume)))               // 3
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    assert(x->i == 42);                                             // 4
    assert(x->s == "hello");                                        // 5
    assert(a.load(std::memory_order_relaxed) == 99);                // 6
}

int main() {
    std::thread t1(create_x);
    std::thread t2(use_x);
    t1.join();
    t2.join();
}

/**
 * 对a的存储①在存储p②之前，并且存储p的操作标记为memory_order_release，加载p的操作标记为memory_order_consume，这就意味着存储p仅先行那些需要加载p的操作。同
 * 样也意味着X结构体中数据成员所在的断言语句④⑤，不会被触发，这是因为对x变量操作的表达式对加载p的操作携带有依赖。另一方面，对于加载变量a的断言就不能确定是
 * 否会被触发；这个操作并不依赖于p的加载操作，所以这里没法保证数据已经被读取。当然，这个情况也是很明显的，因为这个操作被标记为memory_order_relaxed。
 *
 *
 */