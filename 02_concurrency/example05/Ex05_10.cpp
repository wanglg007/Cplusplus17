//使用分离引用计数从无锁栈中弹出一个节点

template<typename T>
class lock_free_stack {
private:
    void increase_head_count(counted_node_ptr &old_counter) {
        counted_node_ptr new_counter;
        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!head.compare_exchange_strong(old_counter, new_counter));
        old_counter.external_count = new_counter.external_count;
    }

public:
    std::shared_ptr <T> pop() {
        counted_node_ptr old_head = head.load();
        for (;;) {
            increase_head_count(old_head);
            node *const ptr = old_head.ptr;                           // 2
            if (!ptr) {
                return std::shared_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next))    // 3
            {
                std::shared_ptr <T> res;
                res.swap(ptr->data);                                // 4
                int const count_increase = old_head.external_count - 2;// 5
                if (ptr->internal_count.fetch_add(count_increase) == -count_increase) { // 6
                    delete ptr;
                }
                return res;                                          // 7
            } else if (ptr->internal_count.fetch_sub(1) == 1) {
                delete ptr;                                          // 8
            }
        }
    }
};

/**
 * 当加载head的值之后，就必须将外部引用加一，是为了表明这个节点正在引用，并且保证在
解引用时的安全性。在引用计数增加前解引用指针，那么就会有线程能够访问这个节点，从
而当前引用指针就成为了一个悬空指针。这就是将引用计数分离的主要原因：通过增加外部
引用计数，保证指针在访问期间的合法性。在compare_exchange_strong()的循环中①完成增
加，通过比较和设置整个结构体来保证指针不会在同一时间内被其他线程修改。
 *
 * 当计数增加，就能安全的解引用ptr，并读取head指针的值，就能访问指向的节点②。如果指
针是空指针，那么将会访问到链表的最后。当指针不为空时，就能尝试对head调用
compare_exchange_strong()来删除这个节点③。
 *
 * 当compare_exchange_strong()成功时，就拥有对应节点的所有权，并且可以和data进行交换
④，然后返回。这样数据就不会持续保存，因为其他线程也会对栈进行访问，所以会有其他指
针指向这个节点。而后，可以使用原子操作fetch_add⑥，将外部计数加到内部计数中去。如
果现在引用计数为0，那么之前的值(fetch_add返回的值)，在相加之前肯定是一个负数，这种
情况下就可以将节点删除。这里需要注意的是，相加的值要比外部引用计数少2⑤;当节点已经
从链表中删除，就要减少一次计数，并且这个线程无法再次访问指定节点，所以还要再减
一。无论节点是否被删除，都能完成操作，所以可以将获取的数据进行返回⑦。
 *
 * 当“比较/交换”③失败，就说明其他线程在之前把对应节点删除了，或者其他线程添加了一个新
的节点到栈中。无论是哪种原因，需要通过“比较/交换”的调用，对具有新值的head重新进行
操作。不过，首先需要减少节点(要删除的节点)上的引用计数。这个线程将再也没有办法访问
这个节点了。如果当前线程是最后一个持有引用(因为其他线程已经将这个节点从栈上删除了)
的线程，那么内部引用计数将会为1，所以减一的操作将会让计数器为0。这样，你就能在循
环⑧进行之前将对应节点删除了。
 *
 */

