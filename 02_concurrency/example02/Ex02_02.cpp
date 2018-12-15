//std::queue 接口
#include <deque>
template<class T, class Container=std::deque <T>>
class queue {
public:
    explicit queue(const Container &);

    explicit queue(Container && = Container());

    queue(queue &&q);

    template<class Alloc>
    explicit queue(const Alloc &);

    template<class Alloc>
    queue(const Container &, const Alloc &);

    template<class Alloc>
    queue(Container &&, const Alloc &);

    template<class Alloc>
    queue(queue &&, const Alloc &);

    queue &operator=(queue &&q);

    void swap(queue &&q);

    bool empty() const;

    size_t size() const;

    T &fromt();

    const T &front() const;

    T &back();

    const T &back() const;

    void push(const T &x);

    void push(T &&x);

    void pop();
};

int main() {

}

/**
 * 当忽略构造、赋值以及交换操作时，就剩下了三组操作：(1)对整个队列的状态进行查询(empty()和size());(2)查询在队列中的各
 * 个元素(front()和back())；(3)修改队列的操作(push(),pop()和emplace())。
 * 因此会遇到在固有接口上的条件竞争。因此需要将front()和pop()合并成一个函数调用。
 */