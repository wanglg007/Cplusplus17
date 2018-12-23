//无锁队列中释放节点外部计数器
#include <atomic>
#include <memory>

template<typename T>
class lock_free_queue {
private:
    struct node;
    struct counted_node_ptr {
        int external_count;
        node *ptr;
    };

    struct node {
        std::shared_ptr<T> data;
        std::atomic<int> internal_count;
        counted_node_ptr next;

        node(T const &data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
    };

    static void free_external_counter(counted_node_ptr &old_node_ptr) {
//        node *const ptr = old_node_ptr.ptr;
//        int const count_increase = old_node_ptr.external_count - 2;
//        node_counter old_counter = ptr->count.load(std::memory_order_relaxed);
//        node_counter new_counter;
//        do {
//            new_counter = old_counter;
//            --new_counter.external_counters;                        // 1
//            new_counter.internal_count += count_increase;           // 2
//        } while (!ptr->count.compare_exchange_strong(old_counter, new_counter,
//                                                     std::memory_order_acquire, std::memory_order_relaxed));    // 3
//        if (!new_counter.internal_count &&
//            !new_counter.external_counters) {
//            delete ptr;                                             // 4
//        }
    }
};

int main() {}

/**
 * 与increase_external_count()对应的是free_external_counter()。这里的代码和Ex05_10中的lock_free_stack::pop()类似，不过做了
 * 一些修改用来处理external_counters计数。使用单个compare_exchange_strong()对计数结构体中的两个计数器进行更新③，就像之前
 * release_ref()降低internal_count一样。internal_count会进行更新②，并且external_counters将会减一①。当内外计数值都为0，
 * 就没有更多的节点可以被引用，所以节点就可以安全的删除④。这个操作需要作为独立的操作来完成(因此需要“比较/交换”循环)，
 * 来避免条件竞争。如果将两个计数器分开来更新，在两个线程的情况下，可能都会认为自己最后一个引用者，从而将节点删除，最后导
 * 致未定义行为。
 *
 * 虽然现在的队列工作正常，且无竞争，但是还是有性能问题。当一个线程对old_tail.ptr->data成功的完成compare_exchange_strong()⑥，
 * 就可以执行push()操作；并且能确定没有其他线程在同时执行push()操作。这里让其他线程看到有新值的加入，要比只看到空指针的好，
 * 因此在compare_exchange_strong()调用失败的时候，线程就会继续循环。这就是忙等待，这种方式会消耗CPU的运算周期，且什么事情
 * 都没做。因此，忙等待这就是一个锁。push()的首次调用，是要在其他线程完成后，将阻塞去除后才能完成，所以这里的实现只是
 * “半无锁”(no longer lock-free)结构。不仅如此，还有当线程被阻塞的时候，操作系统会给不同的线程以不同优先级，用于获取互斥
 * 锁。在当前情况下，不可能出现不同优先级的情况，所以阻塞线程将会浪费CPU的运算周期，直到第一个线程完成其操作。处理的技巧出
 * 自于“无锁技巧包”：等待线程可以帮助push()线程完成操作。
 *
 */

