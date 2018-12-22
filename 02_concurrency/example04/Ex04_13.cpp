//线程安全链表——支持迭代器
#include <memory>
#include <mutex>

template<typename T>
class threadsafe_list {
    struct node {                   // 1
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;

        node() : next() {}          // 2

        node(T const &value) : data(std::make_shared<T>(value)) {}      // 3
    };

    node head;

public:
    threadsafe_list() {}

    ~threadsafe_list() {
        remove_if([](T const &) { return true; });
    }

    threadsafe_list(threadsafe_list const &other) = delete;

    threadsafe_list &operator=(threadsafe_list const &other) = delete;

    void push_front(T const &value) {
        std::unique_ptr<node> new_node(new node(value));        // 4
        std::lock_guard<std::mutex> lk(head.m);
        new_node->next = std::move(head.next);                  // 5
        head.next = std::move(new_node);                        // 6
    }

    template<typename Function>
    void for_each(Function f) {                                 // 7
        node *current = &head;
        std::unique_lock<std::mutex> lk(head.m);                // 8
        while (node *const next = current->next.get()) {      // 9
            std::unique_lock<std::mutex> next_lk(next->m);      // 10
            lk.unlock();                                        // 11
            f(*next->data);                                     // 12
            current = next;
            lk = std::move(next_lk);                            // 13
        }
    }

    template<typename Predicate>
    std::shared_ptr<T> find_first_if(Predicate p) {             // 14
        node *current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node *const next = current->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            lk.unlock();
            if (p(*next->data)) {                               // 15
                return next->data;                             // 16
            }
            current = next;
            lk = std::move(next_lk);
        }
        return std::shared_ptr<T>();
    }

    template<typename Predicate>
    void remove_if(Predicate p) {                               // 17
        node *current = &head;
        std::unique_lock<std::mutex> lk(head.m);
        while (node *const next = current->next.get()) {
            std::unique_lock<std::mutex> next_lk(next->m);
            if (p(*next->data)) {                               // 18
                std::unique_ptr<node> old_next = std::move(current->next);
                current->next = std::move(next->next);
                next_lk.unlock();                               // 20
            } else {
                lk.unlock();                                    // 21
                current = next;
                lk = std::move(next_lk);
            }
        }
    }
};

int main() {}

/**
 * threadsafe_list<>是个单链表，可从node的结构①看出:一个默认构造的node作为链表的head，其next指针②指向的是NULL。新节点都是被push_front()函数添加
 * 进去的；构造第一个新节点④，其将会在堆上分配内存③来对数据进行存储，同时将next指针置为NULL。然后需要获取head节点的互斥锁，为了让设置next的值⑤，
 * 也就是插入节点到列表的头部，让头节点的head.next指向这个新节点⑥。目前还没有什么问题：只需要锁住一个互斥量，就能将一个新的数据添加进入链表，所以
 * 这里不存在死锁的问题。同样，(缓慢的)内存分配操作在锁的范围外，所以锁能保护需要更新的一对指针。
 *
 * 首先看一下for_each()⑦:这个操作需要对队列中的每个元素执行Function(函数指针)；在大多数标准算法库中都会通过传值方式来执行这个函数，这里要不就传
 * 入一个通用的函数，要不就传入一个有函数操作的类型对象。在这种情况下，这个函数必须接受类型为T的值作为参数。在链表中会有一个“手递手”的上锁过程。
 * 在这个过程开始时，需要锁住head及节点⑧的互斥量。然后安全的获取指向下一个节点的指针(使用get()获取，这是因为你对这个指针没有所有权)。当指针不为
 * NULL⑨，为了继续对数据进行处理，就需要对指向的节点进行上锁⑩。当已经锁住了那个节点，就可以对上一个节点进行释放了⑪，并且调用指定函数⑫。当函数
 * 执行完成时就可以更新当前指针所指向的节点(刚刚处理过的节点)，并且将所有权从next_lk移动移动到lk⑬。因为for_each传递的每个数据都是能被Function接受的，
 * 所以当需要时，需要拷贝到另一个容器的时，或其他情况时，你都可以考虑使用这种方式更新每个元素。如果函数的行为没什么问题，这种方式是完全安全的，因为
 * 在获取节点互斥锁时，已经获取锁的节点正在被函数所处理。
 *
 * find_first_if()⑭和for_each()很相似；最大的区别在于find_first_if支持函数(谓词)在匹配的时候返回true，在不匹配的时候返回false⑮。当条件匹配，只需要返回
 * 找到的数据⑯，而非继续查找。
 * remove_if()⑰就有些不同，因为这个函数会改变链表；所以就不能使用for_each()来实现这个功能。当函数(谓词)返回true⑱，对应元素将会移除，并且更新current->next⑲。
 * 当这些都做完，你就可以释放next指向节点的锁。当std::unique_ptr<node> 的移动超出链表范围⑳，这个节点将被删除。这种情况下就不需要更新当前节点，因为只需要
 * 修改next所指向的下一个节点就可以。当函数(谓词)返回false，那么移动的操作就和之前一样(21)。
 * 那么所有的互斥量中会有死锁或条件竞争吗？答案无疑是“否”，这里要看提供的函数(谓词)是否有良好的行为。迭代通常都是使用一种方式，都是从head节点开始，并且在
 * 释放当前节点锁之前将下一个节点的互斥量锁住，所以这里就不可能会有不同线程有不同的上锁顺序。唯一可能出现条件竞争的地方就是在remove_if()⑳中删除已有节点的时
 * 候。因为这个操作在解锁互斥量后进行(其导致的未定义行为，可对已上锁的互斥量进行破坏)。不过在考虑一阵后可以确定这的确是安全的，因为你还持有前一个节点(当前节
 * 点)的互斥锁，所以不会有新的线程尝试去获取你正在删除的那个节点的互斥锁。
 *
 * 这里并发概率有多大呢？细粒度锁要比单锁的并发概率大很多，那我们已经获得了吗？是的：同一时间内，不同线程可以在不同节点上工作，无论是其使用for_each()对每一个
 * 节点进行处理，使用find_first_if()对数据进行查找，还是使用remove_if()删除一些元素。不过，因为互斥量必须按顺序上锁，那么线程就不能交叉进行工作。当一个线程耗
 * 费大量的时间对一个特殊节点进行处理，那么其他线程就必须等待这个处理完成。在完成后，其他线程才能到达这个节点。
 *
 */