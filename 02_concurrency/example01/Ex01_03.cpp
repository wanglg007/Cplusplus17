//(1.3)std::stack容器的实现
#include <deque>

template<typename T, typename Container=std::deque<T>>
class stack {
public:
    explicit stack(const Container &);

    explicit stack(Container && = Container());

    template<class Alloc>
    explicit stack(const Alloc &);

    template<class Alloc>
    stack(const Container &, const Alloc &);

    template<class Alloc>
    stack(Container &&, const Alloc &);

    template<class Alloc>
    stack(stack &&, const Alloc &);

    bool empty() const;

    size_t size() const;

    T &top();

    T const &top() const;

    void push(T const &);

    void push(T &&);

    void pop();

    void swap(stack &&);
};

int main() {

}

/**
 * 构建类似于std::stack结构的栈，除了构造函数和swap()以外，需要对std::stack提供五个操作：push()一个新元素进栈，pop()一个元素出栈，
 * top()查看栈顶元素，empty()判断栈是否是空栈，size()有多少个元素。即使修改top()，使其返回一个拷贝而非引用，对内部数据使用
 * 一个互斥量进行保护，不过该接口仍存在条件竞争。该问题不仅存在于基于互斥量实现的接口中，在无锁实现的接口中，条件竞争依旧会产生。
 *
 * 当栈实例是非共享的，如果栈非空，使用empty()检查再调用top()访问栈顶部的元素是安全的。如下代码所示：
 *  stack<int> s;
 *  if (! s.empty()){               // 1
 *   int const value = s.top();     // 2
 *   s.pop();                       // 3
 *   do_something(value);
 *   }
 * 以上是单线程安全代码：对空栈使用top()是未定义行为。对于共享的栈对象，这样的调用顺序就不再安全，因为在调用empty()①和调用top()②之间，可能有来自另
 * 一个线程的pop()调用并删除了最后一个元素。这是经典的条件竞争，使用互斥量对栈内部数据进行保护，但依旧不能阻止条件竞争的发生，这是接口固有问题。
 *
 * (1)传入一个引用:将变量的引用作为参数，传入pop()函数中获取想要的“弹出值”：
 * std::vector<int> result;
 * some_stack.pop(result);
 * 需要临时构造出一个堆中类型的实例用于接收目标值。对于一些类型是不现实，因为从时间和资源的角度上来看，临时构造一个实例是不划算。对于其他的类型，
 * 构造函数需要的一些参数在代码的这个阶段不一定可用。
 * (2)无异常抛出的拷贝构造函数或移动构造函数
 * 对于有返回值的pop()函数来说，只有“异常安全”方面的担忧。很多类型拷贝构造函数不会抛出异常，随着新标准中对“右值引用”的支持，很多
 * 类型都将会有移动构造函数，即使他们和拷贝构造函数做相同的事情，它也不会抛出异常。有用的选项可以限制对线程安全的栈的使用，并且能让栈安全
 * 的返回所需的值，而不会抛出异常。
 * (3)返回指向弹出值的指针
 * 返回指向弹出元素的指针，而不是直接返回值。指针的优势是自由拷贝，并且不会产生异常，这样就能避免Cargill提到的异常问题。缺点就是返回一个指针需要对
 * 对象的内存分配进行管理，对于简单数据类型(比如：int)，内存管理的开销要远大于直接返回值。对于选择该方案的接口，使用std::shared_ptr是个不错的选择；
 * 不仅能避免内存泄露，而且标准库能够完全控制内存分配方案。
 * (4)选项4：“选项1 + 选项2”或 “选项1 + 选项3”
 * 对于通用的代码来说，灵活性不应忽视。当已经选择了选项2或3时，再去选择1也是很容易的。这些选项提供给用户，让用户自己选择最经济的方案。
 */