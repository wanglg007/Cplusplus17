//使用分离引用计数的方式推送一个节点到无锁栈中
#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack {
private:
    struct node;
    struct counted_node_ptr {               // 1
        int external_count;
        node *ptr;
    };

    struct node {
        std::shared_ptr<T> data;
        std::atomic<int> internal_count;    // 2
        counted_node_ptr next;               // 3

        node(T const &data_) : data(std::make_shared<T>(data_)), internal_count(0) {}
    };

    std::atomic<counted_node_ptr> head;     // 4
public:
    ~lock_free_stack() {
        while (pop());
    }

    void push(T const &data) {            // 5
        counted_node_ptr new_node;
        new_node.ptr = new node(data);
        new_node.external_count = 1;
        new_node.ptr->next = head.load();
        while (!head.compare_exchange_weak(new_node.ptr->next, new_node));
    }

    bool pop();
};

int main() {}

/**
 * 外部计数包含在counted_node_ptr的指针中①，且这个结构体会被node中的next指针③和内部计数②用到。counted_node_ptr是个简单
 * 的结构体，所以可以使用特化std::atomic<>模板来对链表的头指针进行声明④。
 *
 * push()相对简单⑤，可以构造一个counted_node_ptr实例，去引用新分配出来的(带有相关数据的)node，并且将node中的next指针设置
 * 为当前head。之后使用compare_exchange_weak()对head的值进行设置。因为internal_count刚被设置，所以其值为0，并且external_count是1。
 * 因为这是一个新节点，那么这个节点只有一个外部引用(head指针)。
 *
 */