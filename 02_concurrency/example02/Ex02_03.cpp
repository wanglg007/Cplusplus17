//线程安全队列的接口
#include <memory>

template<typename T>
class threadsafe_queue {
public:
    threadsafe_queue();

    threadsafe_queue(const threadsafe_queue &);

    threadsafe_queue &operator=(const threadsafe_queue &) = delete; // 不允许简单的赋值

    void push(T new_value);

    bool try_pop(T &value);                                             // 1

    std::shared_ptr<T> try_pop();                                       // 2

    void wait_and_pop(T &value);

    std::shared_ptr<T> wait_and_pop();

    bool empty() const;
};

int main() {

}

/**
 * 需要提供两个版本的try_pop()和wait_for_pop()。第一个重载的try_pop()①在引用变量中存储着检索值，所以它可以用来返回队列中值的状态；当检索到一个变量时，
 * 他将返回true，否则将返回false。第二个重载②就不能做这样，因为它是用来直接返回检索值的。当没有值可检索时，这个函数可以返回NULL指针。
 */