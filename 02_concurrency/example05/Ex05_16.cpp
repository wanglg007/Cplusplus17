//在无锁队列中释放一个节点引用
template<typename T>
class lock_free_queue {
private:
    struct node {
        void release_ref() {
            node_counter old_counter = count.load(std::memory_order_relaxed);
            node_counter new_counter;
            do {
                new_counter = old_counter;
                --new_counter.internal_count;           // 1
            } while (!count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire,
                                                    std::memory_order_relaxed));        // 2
            if (!new_counter.internal_count &&
                !new_counter.external_counters) {
                delete this;                        // 3
            }
        }
    };
};

/**
 * node::release_ref()的实现，只是对Ex05_10中lock_free_stack::pop()进行小幅度的修改得到。不
过，7.11中的代码仅是处理单个外部计数的情况，所以想要修改internal_count①，只需要使
用fetch_sub就能让count结构体自动更新。因此，需要一个“比较/交换”循环②。降低
internal_count时，在内外部计数都为0时，就代表这是最后一次引用，之后就可以将这个节点
删除③。

 *
 *
 */

