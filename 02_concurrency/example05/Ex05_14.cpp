//使用带有引用计数tail，实现的无锁队列中的push()
#include <atomic>

template<typename T>
class lock_free_queue {
private:
    struct node;
    struct counted_node_ptr {
        int external_count;
        node *ptr;
    };
    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;     // 1
    struct node_counter {
        unsigned internal_count:30;
        unsigned external_counters:2;     // 2
    };

    struct node {
        std::atomic<T *> data;
        std::atomic<node_counter> count;   // 3
        counted_node_ptr next;

        node() {
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;// 4
            count.store(new_count);
            next.ptr = nullptr;
            next.external_count = 0;
        }
    };

public:
    void push(T new_value) {
        std::unique_ptr<T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();
        for (;;) {
            increase_external_count(tail, old_tail);            // 5
            T *old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {     // 6
                old_tail.ptr->next = new_next;
                old_tail = tail.exchange(new_next);
                free_external_counter(old_tail);                // 7
                new_data.release();
                break;
            }
            old_tail.ptr->release_ref();
        }
    }
};

/**
 * tail和head一样都是atomic类型①，并且node结构体中用count成员变量替换了
之前的internal_count③。count成员变量包括了internal_count和外部external_counters成员
②。注意，这里你需要2bit的external_counters，因为最多就有两个计数器。因为使用了位
域，所以就将internal_count指定为30bit的值，就能保证计数器的总体大小是32bit。内部计数
值就有充足的空间来保证这个结构体能放在一个机器字中(包括32位和64位平台)。重要的是，
为的就是避免条件竞争，将结构体作为一个单独的实体来更新。让结构体的大小保持在一个
机器字内，对其的操作就如同原子操作一样，还可以在多个平台上使用。
 *
 * node初始化时，internal_count设置为0，external_counter设置为2④，因为当新节点加入队
列中时，都会被tail和上一个节点的next指针所指向。push()与清单7.14中的实现很相似，除
了为了对tail中的值进行解引用，需要调用节点data成员变量的compare_exchange_strong()
成员函数⑥保证值的正确性；在这之前还要调用increase_external_count()增加计数器的计数
⑤，而后在对尾部的旧值调用free_external_counter()⑦。
 *
 *
 *
 */
