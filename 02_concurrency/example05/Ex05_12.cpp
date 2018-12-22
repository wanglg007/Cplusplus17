//单生产者/单消费者模型下的无锁队列
#include <memory>
#include <atomic>

template<typename T>
class lock_free_queue {
private:
    struct node {
        std::shared_ptr<T> data;
        node *next;

        node() :
                next(nullptr) {}
    };

    std::atomic<node *> head;
    std::atomic<node *> tail;

    node *pop_head() {
        node *const old_head = head.load();     // 1
        if (old_head == tail.load()) {
            return nullptr;
        }
        head.store(old_head->next);
        return old_head;
    }

public:
    lock_free_queue() :
            head(new node), tail(head.load()) {}

    lock_free_queue(const lock_free_queue &other) = delete;

    lock_free_queue &operator=(const lock_free_queue &other) = delete;

    ~lock_free_queue() {
        while (node *const old_head = head.load()) {
            head.store(old_head->next);
            delete old_head;
        }
    }

    std::shared_ptr<T> pop() {
        node *old_head = pop_head();
        if (!old_head) {
            return std::shared_ptr<T>();
        }
        std::shared_ptr<T> const res(old_head->data);           // 2
        delete old_head;
        return res;
    }

    void push(T new_value) {
        std::shared_ptr<T> new_data(std::make_shared<T>(new_value));
        node *p = new node;                                     // 3
        node *const old_tail = tail.load();                     // 4
        old_tail->data.swap(new_data);                          // 5
        old_tail->next = p;                                     // 6
        tail.store(p);                                          // 7
    }
};

int main() {}

/**
 * 当只有一个线程调用一次push()，且只有一个线程调用
pop()。在这种情况下，队列完美工作。push()和pop()之间的先行关系就很重要了，这直接关
系到获取到的data。对tail的存储⑦同步于对tail的加载①；存储之前节点的data指针⑤先行于
存储tail；并且，加载tail先行于加载data指针②，所以对data的存储要先行于加载，一切都没
问题。因此，这是一个完美的“单生产者，单消费者”(single-producer, single-consumer，
SPSC)队列。
 *
 * 问题在于当多线程对push()或pop()并发调用。先看一下push()：如果有两个线程并发调用
push()，那么它们会新分配两个节点作为虚拟节点③，也会读取到相同的tail值④，因此也会同
时修改同一个节点，同时设置data和next指针⑤⑥。明显的数据竞争！
 *
 * pop_head()函数也有类似的问题。当有两个线程并发的调用这个函数时，这两个线程就会读
取到同一个head中同样的值，并且会同时通过next指针去复写旧值。两个线程现在都能索引
到同一个节点——真是一场灾难！这里，不仅要保证只有一个pop()线程可以访问给定项，还
要保证其他线程在读取head指针时，可以安全的访问节点中的next。这就和无锁栈中pop()的
问题一样了，那么就有很多解决方案可以在这里使用。
 *
 * pop()的问题解决了，那么push()呢？问题在于为了获取push()和pop()间的先行关系，就需要
在为虚拟节点设置数据项前，更新tail指针。这就意味着，并发访问push()时，因为每个线程
所读取到的是同一个tail指针，所以线程会为同一个数据项进行竞争。
 *
 */
