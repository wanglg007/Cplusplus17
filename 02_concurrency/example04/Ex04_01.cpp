//线程安全栈的类定义
#include <exception>
#include <stack>
#include <mutex>
#include <memory>

struct empty_stack : std::exception {
    const char *what() const throw() {
        return "empty stack";
    }
};

template<typename T>
class threadsafe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    threadsafe_stack() {}

    threadsafe_stack(const threadsafe_stack &other) {
        std::lock_guard<std::mutex> lock(other.m);
        data = other.m;
    }

    threadsafe_stack &operator=(const threadsafe_stack &) = delete;

    void push(T new_value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(std::move(new_value));                // 1
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty())throw empty_stack();          // 2
        std::shared_ptr<T> const res(std::make_shared<T>(std::move(data.pop())));   // 3
        data.pop();                                     // 4
        return res;
    }

    void pop(T &value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) throw empty_stack();
        value = std::move(data.pop());                  // 5
        data.pop();                                     // 6
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

int main() {

}

/**
 * (1)首先互斥量m能保证基本的线程安全，那就是对每个成员函数进行加锁保护。这保证在同一时间内，只有一个线程可以访问到数据，所以能够保证
 * 数据结构的“不变量”被破坏时，不会被其他线程看到。
 * (2)其次在empty()和pop()成员函数之间会存在潜在的竞争，不过代码会在pop()函数上锁时，显式的查询栈是否为空，所以这里的竞争是非恶性的。
 * pop()通过对弹出值的直接返回，就可避免std::stack<>中top()和pop()两成员函数之间的潜在竞争。
 * (3)再次这个类中也有一些异常源。对互斥量上锁可能会抛出异常，因为上锁操作是每个成员函数所做的第一个操作，所以这是极其罕见的(因为这意味
 * 这问题不在锁上，就是在系统资源上)。因无数据修改，所以其是安全的。因解锁一个互斥量是不会失败的，所以段代码很安全，并且使用std::lock_guard<>
 * 也能保证互斥量上锁的状态。
 *
 * (1)对data.push()①的调用可能会抛出一个异常，不是拷贝/移动数据值时，就是内存不足的时候。不管是哪种，std::stack<>都能保证其实安全的，所以这里也没有问题。
 * (2)在第一个重载pop()中，代码可能会抛出一个empty_stack的异常②，不过数据没有被修改，所以其是安全的。对于res的创建③也可能会抛出一个异常，这有两方
 * 面的原因：对std::make_shared的调用可能无法分配出足够的内存去创建新的对象，并且内部数据需要对新对象进行引用；或者，在拷贝或移动构造到新分配的内
 * 存中返回时抛出异常。两种情况下，c++运行库和标准库能确保这里不会出现内存泄露，并且新创建的对象(如果有的话)都能被正确销毁。因为没有对栈进行任何
 * 修改，所以这里也不会有问题。当调用data.pop()④时，其能确保不抛出异常，并且返回结果，所以这个重载pop()函数“异常-安全”。
 * 第二个重载pop()类似，除了在拷贝赋值或移动赋值的时候会抛出异常⑤，当构造一个新对象和一个std::shared_ptr实例时都不会抛出异常。同样在调用data.pop()⑥
 * （这个成员函数保证不会抛出异常）之前，依旧没有对数据结构进行修改，所以这个函数也为“异常-安全”。
 * (3)最后，empty()也不会修改任何数据，所以也是“异常-安全”函数。
 *
 * 在调用用户代码会持有锁，所以这里有两个地方可能会产生死锁：进行拷贝构造或移动构造(①，③)时，和在对数据项进行拷贝赋值或移动赋值操作⑤的时候；还有一个
 * 潜在死锁的地方在于用户定义的操作符new。当这些函数无论是以直接调用栈的成员函数的方式，还是在成员函数进行操作时，对已经插入或删除的数据进行操作的方式，
 * 对锁进行获取，都可能造成死锁。不过用户要对栈负责，当栈未对一个数据进行拷贝或分配时，用户就不能想当然的将其添加到栈中。
 *
 * 所有成员函数都使用st::lack_guard<> 来保护数据，所以栈的成员函数能有“线程安全”的表现。当然构造与析构函数不是“线程安全”的，不过这也不成问题，因为
 * 对实例的构造与析构只能有一次。调用一个不完全构造对象或是已销毁对象的成员函数，无论在那种编程方式下都不可取。所以，用户就要保证在栈对象完成构建前，其
 * 他线程无法对其进行访问；并且，一定要保证在栈对象销毁后，所有线程都要停止对其进行访问。
 *
 * 即使在多线程情况下，并发的调用成员函数是安全的(因为使用锁)，也要保证在单线程的情况下，数据结构做出正确反应。序列化线程会隐性的限制程序性能，这就是栈争
 * 议声最大的地方：当一个线程在等待锁时，它就会无所事事。同样的，对于栈来说，等待添加元素也是没有意义的，所以当一个线程需要等待时，其会定期检查empty()或pop()，
 * 以及对empty_stack异常进行关注。这样的现实会限制栈的实现的方式，在线程等待的时候，会浪费宝贵的资源去检查数据，或是要求用户写写外部等待和提示代码(例如，
 * 使用条件变量)，这就使内部锁失去存在的意义——这就意味着资源的浪费。
 */
