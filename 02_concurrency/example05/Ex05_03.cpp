//没有线程通过pop()访问节点时，就对节点进行回收
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

    std::atomic<node *> head;
    std::atomic<unsigned> threads_in_pop;           // 1 原子变量

    void try_reclaim(node *old_head);

public:
    std::shared_ptr<T> pop() {
        ++threads_in_pop;                           // 2 在做事之前，计数值加1
        node *old_head = head.load();
        while (old_head && !head.compare_exchange_weak(old_head, old_head->next));
        std::shared_ptr<T> res;
        if (old_head) {
            res.swap(old_head->data);               // 3 回收删除的节点
        }
        try_reclaim(old_head);                      // 4 从节点中直接提取数据，而非拷贝指针
        return res;
    }
};

int main() {}

/**
 * threads_in_pop①原子变量用来记录有多少线程试图弹出栈中的元素。当pop()②函数调用的时候，计数器加一；当调用try_reclaim()时，计数器减一，当这个函数被
 * 节点调用时，说明这个节点已经被删除④。因为暂时不需要将节点删除，可以通过swap()函数来删除节点上的数据③(而非只是拷贝指针)，当不再需要这些数据的时候，
 * 这些数据会自动删除，而不是持续存在着(因为这里还有对未删除节点的引用)。
 *
 *
 */