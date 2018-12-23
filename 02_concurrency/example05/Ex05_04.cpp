//采用引用计数的回收机制
#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack {
private:
    struct node {
        std::shared_ptr<T> data;
        node *next;

        node(T const &data_) : data(std::make_shared<T>(data_)) {}
    };

    std::atomic<unsigned> threads_in_pop;
    std::atomic<node *> to_be_deleted;

    static void delete_nodes(node *nodes) {
        while (nodes) {
            node *next = nodes->next;
            delete nodes;
            nodes = next;
        }
    }

    void try_reclaim(node *old_head) {
        if (threads_in_pop == 1) {                                       // 1
            node *nodes_to_delete = to_be_deleted.exchange(nullptr);    // 2
            if (!--threads_in_pop) {                                     // 3 是否只有一个线程调用pop()？
                delete_nodes(nodes_to_delete);                           // 4
            } else if (nodes_to_delete) {                               // 5
                chain_pending_nodes(nodes_to_delete);                    // 6
            }
            delete old_head;                                            // 7
        } else {
            chain_pending_node(old_head);                                // 8
            --threads_in_pop;
        }
    }

    void chain_pending_nodes(node *nodes) {
        node *last = nodes;
        while (node *const next = last->next) {                         // 9 让next指针指向链表的末尾
            last = next;
        }
        chain_pending_nodes(nodes, last);
    }

    void chain_pending_nodes(node *first, node *last) {
        last->next = to_be_deleted;                                      // 10
        while (!to_be_deleted.compare_exchange_weak(last->next, first));// 11 用循环来保证
    }

    void chain_pending_node(node *n) {
        chain_pending_nodes(n, n);                                       // 12
    }
};

int main() {}

/**
 * 回收节点时①，threads_in_pop的数值是1，也就是当前线程正在对pop()进行访问，这时就可以安全的将节点进行删除⑦(将等待节点
 * 删除是安全的)。当数值不是1时，删除任何节点都不安全，所以需要向等待列表中继续添加节点⑧。
 *
 * 假设某一时刻threads_in_pop的值为1。那就可以尝试回收等待列表，如果不回收，节点就会继续等待，直到整个栈被销毁。要做到回收，
 * 首先通过原子exchange操作声明②删除列表，并将计数器减一③。如果之后计数的值为0，就意味着没有其他线程访问等待节点链表。出
 * 现新等待节点时不必为其烦恼，因为它们将被安全的回收。而后可以使用delete_nodes对链表进行迭代，并将其删除④。
 *
 * 当计数值在减后不为0，回收节点就不安全；所以如果存在⑤，就需要将其挂在等待删除链表之后⑥，这种情况会发生在多个线程同时访
 * 问数据结构的时候。一些线程在第一次测试threads_in_pop①和对“回收”链表的声明②操作间调用pop()，可能新填入一个已经被线程
 * 访问的节点到链表中。
 *
 * 为了将等待删除的节点添加入等待删除链表，需要复用节点的next指针将等待删除节点链接在一起。这种情况下，将已存在的链表链接
 * 到删除链表后面，通过遍历的方式找到链表的末尾⑨，将最后一个节点的next指针替换为当前to_be_deleted指针⑩，并且将链表中的第
 * 一个节点作为新的to_be_deleted指针进行存储⑪。这里需要在循环中使用compare_exchange_weak来保证，通过其他线程添加进来的节点
 * 不会发生内存泄露。在链表发生改变时，更新next指针很方便。添加单个节点是一种特殊情况，因为这需要将这个节点作为第一个节点，
 * 同时也是最后一个节点进行添加⑫。
 *
 * 在低负荷的情况下，这种方式没有问题，因为在没有线程访问pop()有一个合适的静态指针。不过这只是一个瞬时的状态，也就是为什么
 * 在回收前，需要检查threads_in_pop计数为0③的原因；同样也是删除节点⑦前进行对计数器检查的原因。删除节点是一项耗时的工作，
 * 并且希望其他线程能对链表做的修改越小越好。从第一次发现threads_in_pop是1，到尝试删除节点，会用很长的时间，这样就会让线程
 * 有机会调用pop()，会让threads_in_pop不为0，阻止节点的删除操作。
 *
 * 在高负荷的情况不会存在静态；因为其他线程在初始化之后都能进入pop()。在这样的情况下，to_ne_deleted链表将会无界的增加，并且
 * 会再次泄露。当这里不存在任何静态的情况时，就得为回收节点寻找替代机制。关键是要确定没有线程访问一个给定节点，那么这个节点就能被回收。
 */
