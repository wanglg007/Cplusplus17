//使用尾部引用计数，将节点从无锁队列中弹出
#include <memory>
#include <atomic>

template<typename T>
class lock_free_queue {
private:
    struct node {
        void release_ref();
    };

    struct counted_node_ptr {
        int external_count;
        node *ptr;
    };

    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;

public:
    std::unique_ptr <T> pop() {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);
        for (;;) {
            increase_external_count(head, old_head);                    // 2
            node *const ptr = old_head.ptr;
            if (ptr == tail.load().ptr) {
                ptr->release_ref();                                     // 3
                return std::unique_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next)) {    // 4
                T *const res = ptr->data.exchange(nullptr);
                free_external_counter(old_head);                        // 5
                return std::unique_ptr<T>(res);
            }
            ptr->release_ref();                                         // 6
        }
    }
};

int main() {}

/**
 * 进入循环，并将加载值的外部计数增加②之前，需要加载old_head值作为启动①。当head与tail节点相同的时候，就能对引用进行释放③，
 * 因为这时队列中已经没有数据，所以返回的是空指针。如果队列中还有数据，可以尝试使用compare_exchange_strong()来做声明④。与
 * Ex05_10，将外部计数和指针做为一个整体进行比较的；当外部计数或指针有所变化时，需要将引用释放后，再次进行循环⑥。当交换成
 * 功时，已声明的数据就归你所有，那么为 已弹出节点释放外部计数后⑤，就能把对应的指针返回给调用函数了。当两个外部引用计数都
 * 被释放，且内部计数降为0时，节点就可以被删除。
 *
 */

