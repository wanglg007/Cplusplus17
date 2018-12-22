//可上锁和等待的线程安全队列——wait_and_pop()
#include <memory>
#include <mutex>
#include <condition_variable>

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
    std::condition_variable data_cond;

    node *get_tail() {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head() {                             // 1
        std::unique_ptr<node> const old_head = std::move(head);
        head = std::move(old_head->next);
        return old_head;
    }

    std::unique_lock<std::mutex> wait_for_data() {                 // 2
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock, [&] { return head != get_tail(); });
        return std::move(head_lock);                                // 3
    }

    std::unique_ptr<node> wait_pop_head() {
        std::unique_lock<std::mutex> head_lock(wait_for_data());   // 4
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head(T &value) {
        std::unique_lock<std::mutex> head_lock(wait_for_data());   // 5
        value = std::move(*head->data);
        return pop_head();
    }

public:
    std::shared_ptr<T> wait_and_pop() {
        std::unique_ptr<node> const old_head = wait_pop_head();
        return old_head->data;
    }

    void wait_and_pop(T &value) {
        std::unique_ptr<node> const old_head = wait_pop_head(value);
    }
};

int main() {}

/**
 * pop_head()①和wait_for_data()②分别是删除头结点和等待队列中有数据弹出的。wait_for_data()不仅等待使用lambda函数对条件变量
 * 进行等待，而且还会将锁的实例返回给调用者③。这就需要确保同一个锁在执行与wait_pop_head()重载④⑤的相关操作时，已持有锁。
 */