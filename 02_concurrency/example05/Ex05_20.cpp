//无锁队列中简单的帮助性push()的实现
template<typename T>
class lock_free_queue {
private:
    void set_new_tail(counted_node_ptr &old_tail, counted_node_ptr const &new_tail) {       // 1
        node *const current_tail_ptr = old_tail.ptr;
        while (!tail.compare_exchange_weak(old_tail, new_tail) && old_tail.ptr == current_tail_ptr);    // 2
        if (old_tail.ptr == current_tail_ptr)       // 3
            free_external_counter(old_tail);        // 4
        else
            current_tail_ptr->release_ref();        // 5
    }

public:
    void push(T new_value) {
        std::unique_ptr <T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();
        for (;;) {
            increase_external_count(tail, old_tail);
            T *old_data = nullptr;
            if (old_tail.ptr->data.compare_exchange_strong(old_data, new_data.get())) {     // 6
                counted_node_ptr old_next = {0};
                if (!old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {      // 7
                    delete new_next.ptr;        // 8
                    new_next = old_next;        // 9
                }
                set_new_tail(old_tail, new_next);
                new_data.release();
                break;
            } else {                        // 10
                counted_node_ptr old_next = {0};
                if (old_tail.ptr->next.compare_exchange_strong(old_next, new_next)) {   // 11
                    old_next = new_next;        // 12
                    new_next.ptr = new node;    // 13
                }
                set_new_tail(old_tail, old_next);// 14
            }
        }
    }
};

/**
 * 与清单7.15中的原始push()相似，不过还是有些不同。当对data进行设置⑥，就需要对另一线
程帮忙的情况进行处理，在else分支就是具体的帮助⑩。
 *
 * 对节点中的data指针进行设置⑥时，新版push()对next指针的更新使用的是
compare_exchange_strong()⑦(这里使用compare_exchange_strong()来避免循环),当交换失
败，就能知道另有线程对next指针进行设置，所以就可以删除一开始分配的那个新节点⑧。还
需要获取next指向的值——其他线程对tail指针设置的值。
 *
 *对tail指针的更新，实际在set_new_tail()中完成①。这里使用一个compare_exchange_weak()
循环②来更新tail，如果其他线程尝试push()一个节点时，external_count部分将会改变。不
过，当其他线程成功的修改了tail指针时，就不能对其值进行替换；否则，队列中的循环将会
结束，这是一个相当糟糕的主意。因此，当“比较/交换”操作失败的时候，就需要保证ptr加载
值要与tail指向的值相同。当新旧ptr相同时，循环退出③，这就代表对tail的设置已经完成，所
以需要释放旧外部计数器④。当ptr值不一样时，那么另一线程可能已经将计数器释放了，所
以这里只需要对该线程持有的单次引用进行释放即可⑤。
 *
 *当线程调用push()时，未能在循环阶段对data指针进行设置，那么这个线程可以帮助成功的线
程完成更新。首先，会尝试更新next指针，让其指向该线程分配出来的新节点⑪。当指针更新
成功，就可以将这个新节点作为新的tail节点⑫，且需要分配另一个新节点，用来管理队列中
新推送的数据项⑬。在再进入循环之前，可以通过调用set_new_tail来设置tail节点⑭
 *
 * 比起大量的new和delete操作，这样的代码更加短小精悍，因为新节点
实在push()中被分配，而在pop()中被销毁。因此，内存分配器的效率也需要考虑到；一个糟
糕的分配器可能会让无锁容器的扩展特性消失的一干二净。选择和实现高效的分配器，已经
超出了本书的范围，不过需要牢记的是：测试以及衡量分配器效率最好的办法，就是对使用
前和使用后进行比较。为优化内存分配，包括每个线程有自己的分配器，以及使用回收列表
对节点进行回收，而非将这些节点返回给分配器。
 */
