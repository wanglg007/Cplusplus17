//带有虚拟节点的队列
#include <memory>

template<typename T>
class queue {
private:
    struct node {
        std::shared_ptr<T> data;        // 1
        std::unique_ptr<node> next;
    };

    std::unique_ptr<node> head;
    node *tail;

public:
    queue() :
            head(new node), tail(head.get()) {}     // 2

    queue(const queue &other) = delete;

    queue &operator=(const queue &other) = delete;

    std::shared_ptr<T> try_pop() {
        if (head.get() == tail) {                   // 3
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(head->data);   // 4
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);            // 5
        return res;                                 // 6
    }

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));     // 7
        std::unique_ptr<node> p(new node);                     // 8
        tail->data = new_data;                                  // 9
        node *const new_tail = p.get();
        tail->next = std::move(p);
        tail = new_tail;
    }
};

int main() {}

/**
 * try_pop()不需要太多的修改。首先可以拿head和tail③进行比较，这就比检查指针是否为空的好，因为虚拟节点意味着head不可能是空指针。head是个
 * std::unique_ptr<node>对象，需要使用head.get()来做比较。其次因为node现在存在数据指针中①，你可以对指针进行直接检索④，而非构造一个T类
 * 型的新实例。push()函数改动最大：首先必须在堆上创建一个T类型的实例，并且让其与一个std::shared_ptr<>对象相关联⑦(节点使用std::make_shared
 * 就是为了避免内存二次分配，避免增加引用次数)。创建的新节点就成为虚拟节点，所以不需要为new_value提供构造函数⑧。反而这里需要将new_value的
 * 副本赋给之前的虚拟节点⑨。最终为了让虚拟节点存在在队列中，你不得不使用构造函数来创建它②。
 *
 * 我们的目的是为了最大程度的并发化，所以需要上锁的时间要尽可能的小。push()很简单：互斥量需要对tail的访问进行上锁，这意味着需要对每个新分配
 * 的节点进行上锁⑧，还有在对当前尾节点进行赋值的时候⑨也需要上锁。锁需要持续到函数结束时才能解开。
 *
 * try_pop()就不简单。首先需要使用互斥量锁住head，一直到head弹出。实际上互斥量决定了哪一个线程来进行弹出操作。一旦head被改变⑤才能解锁互斥
 * 量；当在返回结果时，互斥量就不需要进行上锁⑥。这使得访问tail需要一个尾互斥量。因为只需要访问tail一次，且只有在访问时才需要互斥量。这个操作
 * 最好是通过函数进行包装。代码只有在成员需要head时，互斥量才上锁，这项也需要包含在包装函数中。
 *
 */