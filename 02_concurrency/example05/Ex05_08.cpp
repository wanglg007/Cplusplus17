//无锁栈——使用无锁 std::shared_ptr<> 的实现
#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack {
private:
    struct node {
        std::shared_ptr<T> data;
        std::shared_ptr<node> next;

        node(T const &data_) :
                data(std::make_shared<T>(data_)) {}
    };

    std::shared_ptr<node> head;
public:
    void push(T const &data) {
        std::shared_ptr<node> const new_node = std::make_shared<node>(data);
        new_node->next = head.load();
        while (!std::atomic_compare_exchange_weak(&head, &new_node->next, new_node));
    }

    std::shared_ptr<T> pop() {
        std::shared_ptr<node> old_head = std::atomic_load(&head);
        while (old_head && !std::atomic_compare_exchange_weak(&head, &old_head, old_head->next));
        return old_head ? old_head->data : std::shared_ptr<T>();
    }
};

int main() {}

/**
 * 在一些情况下，使用std::shared_ptr<>实现的结构并非无锁，这就需要手动管理引用计数。一种方式是对每个节点使用两个引用计数：内部计数和外部计数。两个值的总和
 * 就是对这个节点的引用数。外部计数记录有多少指针指向节点，即在指针每次进行读取的时候，外部计数加一。当线程结束对节点的访问时，内部计数减一。指针在读取时，
 * 外部计数加一；在读取结束时，内部计数减一。
 *
 */