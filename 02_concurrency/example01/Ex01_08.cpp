//(1.8)简单的层级互斥量实现
#include <mutex>
#include <stdexcept>

class hierarchical_mutex {
    std::mutex internal_mutex;
    unsigned long const hierarchy_value;
    unsigned long previous_hierarchy_value;
    static thread_local unsigned long this_thread_hierarchy_value;          // 1

    void check_for_hierarchy_violation() {
        if (this_thread_hierarchy_value <= hierarchy_value) {                   // 2
            throw std::logic_error("mutex hierarchy violated");
        }
    }

    void update_hierarchy_value() {
        previous_hierarchy_value = this_thread_hierarchy_value;                 // 3
        this_thread_hierarchy_value = hierarchy_value;
    }

public:
    explicit hierarchical_mutex(unsigned long value) :
            hierarchy_value(value),
            previous_hierarchy_value(0) {}

    void lock() {
        check_for_hierarchy_violation();
        internal_mutex.lock();                                              // 4
        update_hierarchy_value();                                           // 5
    }

    void unlock() {
        this_thread_hierarchy_value = previous_hierarchy_value;             // 6
        internal_mutex.unlock();
    }

    bool try_lock() {
        check_for_hierarchy_violation();
        if (!internal_mutex.try_lock())                                     // 7
            return false;
        update_hierarchy_value();
        return true;
    }
};

thread_local unsigned long hierarchical_mutex::this_thread_hierarchy_value(ULLONG_MAX);     // 8

int main() {
    hierarchical_mutex m1(42);
    hierarchical_mutex m2(2000);
}

/**
 * 这里重点使用thread_local的值来代表当前线程的层级值：this_thread_hierarchy_value①。它被初始化为最大值⑧，所以最初所有线程都能被锁住。因
 * 为其声明中有thread_local，所以每个线程都有其拷贝副本，这样线程中变量状态完全独立，当从另一个线程进行读取时，变量的状态也完全独立。
 *
 * 所以，第一次线程锁住一个hierarchical_mutex时，this_thread_hierarchy_value的值是ULONG_MAX。由于其本身的性质，这个值会大于其他任何值，所以会通过
 * check_for_hierarchy_vilation()②的检查。在这种检查方式下，lock()代表内部互斥锁已被锁住④。一旦成功锁住，你可以更新层级值了⑤。
 *
 * 当你现在锁住另一个hierarchical_mutex时，还持有第一个锁，this_thread_hierarchy_value的值将会显示第一个互斥量的层级值。第二个互斥量的层级值必须小于
 * 已经持有互斥量检查函数②才能通过。
 *
 * 最重要的是为当前线程存储之前的层级值，所以你可以调用unlock()⑥对层级值进行保存；否则，就锁不住任何互斥量(第二个互斥量的层级数高于第一个互斥量)，
 * 即使线程没有持有任何锁。因为保存了之前的层级值，只有当持有internal_mutex③，且在解锁内部互斥量⑥之前存储它的层级值，才能安全的将hierarchical_mutex自身
 * 进行存储。这是因为hierarchical_mutex被内部互斥量的锁所保护着。
 *
 * try_lock()与lock()的功能相似，除了在调用internal_mutex的try_lock()⑦失败时，不能持有对应锁，所以不必更新层级值，并直接返回false。
 */