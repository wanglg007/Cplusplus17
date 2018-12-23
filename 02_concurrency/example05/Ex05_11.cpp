//基于引用计数和松散原子操作的无锁栈
#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack {
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

    std::atomic<counted_node_ptr> head;

    void increase_head_count(counted_node_ptr &old_counter) {
        counted_node_ptr new_counter;
        do {
            new_counter = old_counter;
            ++new_counter.external_count;
        } while (!head.compare_exchange_strong(old_counter, new_counter,
                                               std::memory_order_acquire,
                                               std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }

public:
    ~lock_free_stack() {
        while (pop());
    }

    void push(T const &data) {
        counted_node_ptr new_node;
        new_node.ptr = new node(data);
        new_node.external_count = 1;
        new_node.ptr->next = head.load(std::memory_order_relaxed);
        while (!head.compare_exchange_weak(new_node.ptr->next, new_node,
                                           std::memory_order_release,
                                           std::memory_order_relaxed));
    }

    std::shared_ptr<T> pop() {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);
        for (;;) {
            increase_head_count(old_head);
            node *const ptr = old_head.ptr;
            if (!ptr) {
                return std::shared_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next, std::memory_order_relaxed)) {
                std::shared_ptr<T> res;
                res.swap(ptr->data);
                int const count_increase = old_head.external_count - 2;
                if (ptr->internal_count.fetch_add(count_increase, std::memory_order_release) == -count_increase) {
                    delete ptr;
                }
                return res;
            } else if (ptr->internal_count.fetch_add(-1, std::memory_order_relaxed) == 1) {
                ptr->internal_count.load(std::memory_order_acquire);
                delete ptr;
            }
        }
    }

};

int main() {}

/**
 * 做push()的线程：先构造数据项和节点，再设置head。
 * 做pop()的线程：先加载head的值，再在循环中对head做“比较/交换”操作，并增加引用计数，再读取对应的node节点，获取next的指
 * 向的值。next的值是普通的非原子对象，所以为了保证读取安全，必须确定存储(推送线程)和加载(弹出线程)的先行关系。因为唯一的
 * 原子操作就是push()函数中的compare_exchange_weak()，这里需要释放操作来获取两个线程间的先行关系，compare_exchange_weak()
 * 必须是std::memory_order_release或更严格的内存序。当compare_exchange_weak()调用失败，并且可以持续循环，所以使用
 * std::memory_order_relaxed 就足够。
 *
 * 为了确定先行关系，必须在访问next值之前使用std::memory_order_acquire或更严格内存序的操作。因为在increase_head_count()中
 * 使用compare_exchange_strong()就获取next指针指向的旧值，所以想要其获取成功就需要确定内存序。如同调用push()那样，当交换
 * 失败，循环会继续，所以在失败的时候使用松散的内存序。
 *
 */