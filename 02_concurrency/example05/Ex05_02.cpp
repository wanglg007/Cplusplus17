//带有节点泄露的无锁栈
#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack {
private:
    struct node {
        std::shared_ptr<T> data;                        // 1 指针获取数据
        node *next;

        node(T const &data_) :                         // 2 让std::shared_ptr指向新
                data(std::make_shared<T>(data_)) {}
    };

    std::atomic<node *> head;
public:
    void push(T const &data) {
        node *const new_node = new node(data);
        new_node->next = head.load();
        while (!head.compare_exchange_weak(new_node->next, new_node));
    }

    std::shared_ptr<T> pop() {
        node *old_head = head.load();
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next));  // 3 在解引用前检查old_head是否为空指针
        return old_head ? old_head->data : std::shared_ptr<T>();                    // 4
    }
};

int main() {}

/**
 * 智能指针指向当前数据①，这里必须在堆上为数据分配内存(在node结构体中)②。而后在compare_exchage_weak()循环中③，需要在
 * old_head指针前检查指针是否为空。
 * 最终如果存在相关节点，那么将会返回相关节点的值；当不存在时，将返回一个空指针④。注意：结构是无锁的，但并不是无等待的，
 * 因为在push()和pop()函数中都有while循环，当compare_exchange_weak()总是失败的时候，循环将会无限循环下去。
 *
 */

