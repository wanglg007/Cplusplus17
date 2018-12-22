//队列实现——单线程版
#include <memory>

template<typename T>
class queue {
private:
    struct node {
        T data;
        std::unique_ptr<node> next;

        node(T data_) : data(std::move(data_)) {
        }
    };

    std::unique_ptr<node> head;     // 1
    node *tail;                     // 2

public:
    queue() : tail(nullptr) {}

    queue(const queue &other) = delete;

    queue &operator=(const queue &other) = delete;

    std::shared_ptr<T> try_pop() {
        if (!head) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(std::make_shared<T>(std::move(head->data)));
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);                   // 3
        return res;
    }

    void push(T new_value) {
        std::unique_ptr<node> p(new node(std::move(new_value)));
        node *const new_tail = p.get();
        if (tail) {
            tail->next = std::move(p);                      // 4
        } else {
            head = std::move(p);                            // 5
        }
        tail = new_tail;                                    // 6
    }
};

int main() {}

/**
 * 首先使用std::unique_ptr<node>来管理节点，因为其能保证节点(其引用数据的值)在删除时候，不需要使用delete操作显式删除。这样的关系链表
 * 管理着从头结点到尾节点的每一个原始指针。
 *
 * 但是当你在多线程情况下，尝试使用细粒度锁时会出现问题。因为在给定的实现中有两个数据项(head①和tail②)；即使使用两个互斥量来保护头指
 * 针和尾指针也会出现问题。
 *
 * 显而易见的问题就是push()可以同时修改头指针⑤和尾指针⑥，所以push()函数会同时获取两个互斥量。虽然会将两个互斥量都上锁，但这还不是太糟
 * 糕的问题。糟糕的问题是push()和pop()都能访问next指针指向的节点：push()可更新tail->next④，而后try_pop()读取read->next③。当队列中只有
 * 一个元素时，head==tail，所以head->next和tail->next是同一个对象，并且这个对象需要保护。不过，“在同一个对象在未被head和tail同时访问时，
 * push()和try_pop()锁住的是同一个锁”，就不对了。所以就没有比之间实现更好的选择。
 *
 */