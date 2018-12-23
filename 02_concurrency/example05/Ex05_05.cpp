//使用风险指针实现的pop()
#include <atomic>
#include <memory>

void delete_nodes_with_no_hazards();

std::atomic<void *> &get_hazard_pointer_for_current_thread();

template<typename T>
class lock_free_stack {
private:
    struct node {
        std::shared_ptr<T> data;
        node *next;

        node(T const &data_) : data(std::make_shared<T>(data_)) {}
    };

    std::atomic<node *> head;
public:
    std::shared_ptr<T> pop() {
        std::atomic<void *> &hp = get_hazard_pointer_for_current_thread();
        node *old_head = head.load();
        do {                                                         // 1 直到将风险指针设为head指针
            node *temp;
            do {
                temp = old_head;
                hp.store(old_head);
                old_head = head.load();
            } while (old_head != temp);
        } while (old_head && !head.compare_exchange_strong(old_head, old_head->next));
        hp.store(nullptr);                                          // 2 当声明完成，清除风险指针
        std::shared_ptr<T> res;
        if (old_head) {
            res.swap(old_head->data);
            if (outstanding_hazard_pointers_for(old_head)) {        // 3 在删除之前
                reclaim_later(old_head);                            // 4
            } else {
                delete old_head;                                   // 5
            }
            delete_nodes_with_no_hazards();                         // 6
        }
        return res;
    }
};

int main() {}

/**
 * 首先循环内部会对风险指针进行设置，在当“比较/交换”操作失败会重载old_head，再次进行设置①。使用compare_exchange_strong()是
 * 因为需要在循环内部做一些实际的工作：当compare_exchange_weak()伪失败后，风险指针将被重置(没有必要)。该过程能保证风险指针在解
 * 引用(old_head)之前能被正确的设置。当声明了一个风险指针，那么就可以将其清除②。如果想要获取一个节点，就需要检查其他线程上的
 * 风险指针，检查是否有其他指针引用该节点③。如果有，就不能删除节点，只能将其放在链表中，之后再进行回收④；如果没有就能直接将
 * 这个节点删除⑤。最后如果需要对任意节点进行检查，可以调用reclaim_later()。如果链表上没有任何风险指针引用节点，就可以安全的删
 * 除这些节点⑥。当有节点持有风险指针，就只能让下一个调用pop()的线程离开。
 *
 * 为线程分配风险指针实例的具体方案：使用get_hazard_pointer_for_current_thread()与程序逻辑的关系并不大。可以使用一个简单的结
 * 构体：固定长度的“线程ID-指针”数组。get_hazard_pointer_for_curent_thread()可以通过这个数据来找到第一个释放槽，并将当前线
 * 程的ID放入到这个槽中。当线程退出时，槽就再次置空，可以通过默认构造std::thread::id()将线程ID放入槽中。
 */