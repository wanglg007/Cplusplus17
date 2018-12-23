//回收函数的简单实现
#include <atomic>
#include <functional>
#include <thread>

unsigned const max_hazard_pointers = 100;
struct hazard_pointer {
    std::atomic<std::thread::id> id;
    std::atomic<void *> pointer;
};
hazard_pointer hazard_pointers[max_hazard_pointers];

bool outstanding_hazard_pointers_for(void *p) {
    for (unsigned i = 0; i < max_hazard_pointers; ++i) {
        if (hazard_pointers[i].pointer.load() == p) {
            return true;
        }
    }
    return false;
}

template<typename T>
void do_delete(void *p) {
    delete static_cast<T *>(p);
}

struct data_to_reclaim {
    void *data;
    std::function<void(void *)> deleter;
    data_to_reclaim *next;

    template<typename T>
    data_to_reclaim(T *p):                                      // 1
            data(p),
            deleter(&do_delete<T>),
            next(0) {}

    ~data_to_reclaim() {
        deleter(data);                                          // 2
    }
};

std::atomic<data_to_reclaim *> nodes_to_reclaim;

void add_to_reclaim_list(data_to_reclaim *node) {              // 3
    node->next = nodes_to_reclaim.load();
    while (!nodes_to_reclaim.compare_exchange_weak(node->next, node));
}

template<typename T>
void reclaim_later(T *data) {                                  // 4
    add_to_reclaim_list(new data_to_reclaim(data));            // 5
}

void delete_nodes_with_no_hazards() {
    data_to_reclaim *current = nodes_to_reclaim.exchange(nullptr);      // 6
    while (current) {
        data_to_reclaim *const next = current->next;
        if (!outstanding_hazard_pointers_for(current->data)) {           // 7
            delete current;                                             // 8
        } else {
            add_to_reclaim_list(current);                                // 9
        }
        current = next;
    }
}

int main() {}

/**
 * 首先reclaim_later()是一个函数模板④。因为风险指针是一个通用解决方案，所以就不能将栈节点的类型写死。使用std::atomic<void*>
 * 对风险指针进行存储。需要对任意类型的指针进行处理，不过不能使用void*形式，因为当要删除数据项时，delete操作只能对实际类型指
 * 针进行操作。data_to_reclaim的构造函数处理的就很优雅：reclaim_later()只是为指针创建一个data_to_reclaim的实例，并且将实例添
 * 加到回收链表中⑤。add_to_reclaim_list()③使用compare_exchange_weak()循环来访问链表头。
 *
 * 当将节点添加入链表时，data_to_reclaim的析构函数不会被调用；析构函数会在没有风险指针指向节点的时候调用。
 * delete_nodes_with_no_hazards()将已声明的链表节点进行回收，使用的是exchange()函数⑥。这样其他线程就能自由将节点添加到链表中，
 * 或在不影响回收指定节点线程的情况下，对节点进行回收。只要有节点存在于链表中，就需要检查每个节点，查看节点是否被风险指针所指
 * 向⑦。如果没有风险指针，那么就可以安全的将记录删除(并且清除存储的数据)⑧。否则就只能将这个节点添加到链表的后面，再进行回收⑨。
 *
 * 虽然实现很简单，但也安全的回收被删除的节点，不过这个过程增加了很多开销。遍历风险指针数组需要检查max_hazard_pointers原子变量，
 * 并且每次pop()调用时都需要再检查一遍。原子操作很耗时，所以pop()成为性能瓶颈。这种方式不仅需要遍历节点的风险指针链表，还要遍历
 * 等待链表上的每一个节点。当有max_hazard_pointers在链表中，那么就需要检查max_hazard_pointers多个已存储的风险指针。
 */