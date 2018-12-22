//从无锁队列中获取一个节点的引用
template<typename T>
class lock_free_queue {
private:
    static void increase_external_count(
            std::atomic <counted_node_ptr> &counter,
            counted_node_ptr &old_counter) {
        counted_node_ptr new_counter;
        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!counter.compare_exchange_strong(
                old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }
};

/**
 * 这次，并不是对引用的释放，会得到一个新引用，并增加外部
计数的值。increase_external_count()和7.12中的increase_head_count()很相似，不同的是
increase_external_count()这里作为静态成员函数，通过将外部计数器作为第一个参数传入函
数，对其进行更新，而非只操作一个固定的计数器。

 */

