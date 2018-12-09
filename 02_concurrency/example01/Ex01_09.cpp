//在交换操作中使用 std::lock() 和 std::unique_lock
#include <mutex>

class some_big_object {
};

void swap(some_big_object &lhs, some_big_object &rhs) {}

class X {
private:
    some_big_object some_detail;
    mutable std::mutex m;
public:
    X(some_big_object const &sd) : some_detail(sd) {}

    friend void swap(X &lhs, X &rhs) {
        if (&lhs == &rhs)
            return;
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);    // 1 表明互斥量在结构上应该保持解锁状态
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        std::lock(lock_a, lock_b);                                      // 2 互斥量在这里上锁
        swap(lhs.some_detail, rhs.some_detail);
    }
};

int main() {}

/**
 * 将std::unique_lock对象传递到std::lock() ②，这是因为std::unique_lock支持lock(), try_lock()和unlock()成员函数。这些同名的成员函数在互斥低层
 * 做着实际的工作，并且仅更新std::unique_lock实例中的标识，来确定该实例是否拥有特定的互斥量。这个标志确保unlock()在析构函数中被正确调用。如果
 * 实例拥有互斥量，那么析构函数必须调用unlock()；但当实例中没有互斥量时，析构函数就不能去调用unlock()。这个标志可以通过owns_lock()成员变量进行查询。
 *
 * std::unique_lock 对象的体积通常要比std::lock_guard 对象大；当使用 std::unique_lock 替代 std::lock_guard ，因为会对标志进行适当的更新或检查，
 * 就会些轻微的性能惩罚。当 std::lock_guard 已经能够满足你的需求，那么我还是建议你继续使用它。当在需要更加灵活的锁时，你就最好选择 std::unique_lock ，
 * 因为它更适合于你的任务。
 */

