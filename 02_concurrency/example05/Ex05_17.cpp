//从无锁队列中获取一个节点的引用
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

    static void increase_external_count(std::atomic<counted_node_ptr> &counter, counted_node_ptr &old_counter) {
        counted_node_ptr new_counter;
        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!counter.compare_exchange_strong(old_counter, new_counter,
                                                  std::memory_order_acquire, std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }
};

int main() {}
/**
 * 这次并不是对引用的释放，会得到一个新引用，并增加外部计数的值。increase_external_count()和Ex05_11中的increase_head_count()很相似，
 * 不同的是increase_external_count()这里作为静态成员函数，通过将外部计数器作为第一个参数传入函数，对其进行更新，而非只操作一个固定的计数器。
 */

