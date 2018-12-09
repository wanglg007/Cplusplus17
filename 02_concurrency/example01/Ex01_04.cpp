//(1.4)线程安全的堆栈类定义
#include <exception>
#include <memory>       // For std::shared_ptr<>

struct empty_stack : std::exception {
    const char *what() const throw();
};

template<typename T>
class threadsafe_stack {
public:
    threadsafe_stack();

    threadsafe_stack(const threadsafe_stack &);

    threadsafe_stack &operator=(const threadsafe_stack &) = delete;     // (1)

    void push(T new_value);

    std::shared_ptr<T> pop();

    void pop(T &value);

    bool empty() const;
};

int main() {

}

/**
 * 该示例是个接口没有条件竞争的堆栈类定义。它实现了选项1和选项3：重载了pop()，使用一个局部引用去存储弹出值，并返回一个std::shared_ptr<> 对象。
 * 它有一个简单的接口，只有两个函数：push()和pop();
 *
 * 削减接口可以获得最大程度的安全,甚至限制对栈的一些操作。栈是不能直接赋值，因为赋值操作已经删除了①，并且这里没有swap()函数。栈可以拷贝的，假设
 * 栈中的元素可以拷贝。当栈为空时，pop()函数会抛出一个empty_stack异常，所以empty()函数被调用后，其他部件还能正常工作。如选项3描述的那样，使用std::shared_ptr
 * 可以避免内存分配管理的问题，并避免多次使用new和delete操作。堆栈中的五个操作，现在就剩下三个：push(), pop()和empty()(这里empty()都有些多余)。简化
 * 接口更有利于数据控制，可以保证互斥量将一个操作完全锁住。
 *
 *
 *
 */