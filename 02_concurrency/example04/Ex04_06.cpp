//线程安全队列——细粒度锁版
#include <memory>
#include <mutex>

template<typename T>
class threadsafe_queue {
private:
    struct node {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node *tail;

    node *get_tail() {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head() {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if (head.get() == get_tail()) {
            return nullptr;
        }
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

public:
    threadsafe_queue() : head(new node), tail(head.get()) {}

    threadsafe_queue(const threadsafe_queue &other) = delete;

    threadsafe_queue &operator=(const threadsafe_queue &other) = delete;

    std::shared_ptr<T> try_pop() {
        std::unique_ptr<node> old_head = pop_head();
        return old_head ? old_head->data : std::shared_ptr<T>();
    }

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        node *const new_tail = p.get();
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail->data = new_data;
        tail->next = std::move(p);
        tail = new_tail;
    }
};

int main() {}

/**
 * push()很简单：仅修改了被tail_mutex的数据，因为新的尾节点是一个空节点，并且其data和next都为旧的尾节点设置好，所以其能维持不变量的状态。
 *
 * 事实证明:不仅需要对tail_mutex上锁来保护对tail的读取,还要保证在从头读取数据时不会产生数据竞争。如果没有这些互斥量，当一个线程调用try_pop()同时，
 * 另一个线程调用push()，那么操作顺序将不可预测。尽管每一个成员函数都持有一个互斥量，这些互斥量能保护数据不会同时被多个线程访问到；并且队列中的所
 * 有数据来源都是通过调用push()得到的。因为线程可能会无序的访问同一数据，所以就会有数据竞争以及未定义行为。在get_tail()中的tail_mutex解决了所有的
 * 问题。因为调用get_tail()将会锁住同名锁，这就为两个操作规定好顺序。要不就是get_tail()在push()之前被调用，这种情况下，线程可以看到旧的尾节点，要
 * 不就是在push()之后完成，这种情况下，线程就能看到tail的新值，以及新数据前的真正tail的值。
 * 当get_tail()调用前，head_mutex已经上锁，如果不这样，调用pop_head()时就会被get_tail()和head_mutex所卡住，因为其他线程调用try_pop()(以及pop_head())时，
 * 都需要先获取锁，然后阻止从下面的过程中初始化线程：
 *  std::unique_ptr<node> pop_head()    // 这是个有缺陷的实现
 *  {
 *   node* const old_tail=get_tail();   // ① 在head_mutex范围外获取旧尾节点的值
 *   std::lock_guard<std::mutex> head_lock(head_mutex);
 *  if(head.get()==old_tail)            // ②
 *   {
 *   return nullptr;
 *   }
 *   std::unique_ptr<node> old_head=std::move(head);
 *   head=std::move(old_head->next);    // ③
 *   return old_head;
 *   }
 * 这是有缺陷的实现，调用get_tail()是在锁的范围之外，你可能也许会发现head和tail，在初始化线程，并获取head_mutex时发生了改变。并且不只是返回尾节点时，返回的
 * 不是尾节点。即使head是最后一个节点，这也意味着head和old_tail②比较失败。因此当更新head③时，可能会将head移到tail之后，这意味着数据结构遭到了破坏。在正确
 * 实现中需要保证在head_mutex保护的范围内调用get_tail()。这能保证没有其他线程能对head进行修改，并且tail会向正确的方向移动。head不会传递给get_tail()的返回值，
 * 所以不变量的状态是稳定的。
 */