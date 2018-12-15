//使用 std::atomic_flag 实现自旋互斥锁

#include <atomic>

class spinlock_mutex {
    std::atomic_flag flag;
public:
    spinlock_mutex() : flag(ATOMIC_FLAG_INIT) {}

    void lock() {
        while (flag.test_and_set(std::memory_order_acquire));
    }

    void unlock() {
        flag.clear(std::memory_order_release);
    }
};

int main() {

}

/**
 * 有限的特性集使得std::atomic_flag非常适合于作自旋互斥锁。初始化标志是“清除”，并且互斥量处于解锁状态。为了锁上互斥量，循环运行test_and_set()直到旧值为false，
 * 就意味着这个线程已经被设置为true。解锁互斥量是一件很简单的事情，将标志清除即可。
 *
 */