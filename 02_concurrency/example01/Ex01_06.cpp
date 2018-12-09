//(1.6)交换操作中使用 std::lock() 和 std::lock_guard
#include <mutex>
// 这里的std::lock()需要包含<mutex>头文件
class some_big_object {

};

void swap(some_big_object &lhs, some_big_object &rhs) {

}

class X {
private:
    some_big_object some_detail;
    mutable std::mutex m;
public:
    X(some_big_object const &sd) : some_detail(sd) {}

    friend void swap(X &lhs, X &rhs) {
        if (&lhs == &rhs)
            return;
        std::lock(lhs.m, rhs.m);                                    // 1
        std::lock_guard<std::mutex> lock_a(lhs.m, std::adopt_lock); // 2
        std::lock_guard<std::mutex> lock_b(rhs.m, std::adopt_lock); // 3
        swap(lhs.some_detail, rhs.some_detail);
    }
};

int main() {

}

/**
 * 首先检查参数是否是不同的实例，因为操作试图获取std::mutex对象上的锁，所以当其已经被获取时，结果很难预料(一个互斥量可以在同一线程上多次上
 * 锁，标准库中std::recursive_mutex提供这样的功能)。然后调用 std::lock()①锁住两个互斥量，并且两个std:lock_guard 实例已经创建好②③，还有个
 * 互斥量。提供std::adopt_lock参数除了表示std::lock_guard对象已经上锁外，还表示现成的锁，而非尝试创建新的锁。
 *
 * 大多数情况下，函数退出时互斥量能被正确的解锁(保护操作可能会抛出异常)，也允许使用一个简单的“return”作为返回。当使用std::lock去锁lhs.m或rhs.m时，
 * 可能会抛出异常；这种情况下，异常会传播到std::lock之外。当std::lock成功的获取一个互斥量上的锁，并且当其尝试从另一个互斥量上再获取锁时，就会有异常
 * 抛出，第一个锁也会随着异常的产生而自动释放，所以std::lock要么将两个锁都锁住，要不一个都不锁。
 *
 * 虽然std::lock可以在这情况(获取两个以上的锁)避免死锁，但它没办法帮助你获取其中一个锁。这时不得不依赖于作为开发者的纪律性来确保程序不会死锁。
 * 这并不简单：死锁是多线程编程中一个令人相当头痛的问题，并且死锁经常是不可预见。
 */