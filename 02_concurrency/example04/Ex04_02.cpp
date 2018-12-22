//使用条件变量实现的线程安全队列
#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template<typename T>
class threadsafe_queue {
private:
    mutable std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;
public:
    threadsafe_queue() {}

    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(std::move(new_value));
        data_cond.notify_one();                     // 1
    }

    void wait_and_pop(T &value) {                   // 2
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = std::move(data_queue.front());
        data_queue.pop();
    }

    std::shared_ptr<T> wait_and_pop() {             // 3
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });         // 4
        std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }

    bool try_pop(T &value) {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return false;
        value = std::move(data_queue.front());
        data_queue.pop();
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lk(mut);
        if (data_queue.empty())
            return std::shared_ptr<T>();                // 5
        std::shared_ptr<T> res(std::make_shared<T>(std::move(data_queue.front())));
        data_queue.pop();
        return res;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mut);
        return data_queue.empty();
    }
};

int main() {
    threadsafe_queue<int> rq;
}

/**
 * 除了在push()①中调用data_cond.notify_one()，以及wait_and_pop()②③，本例队列的实现与Ex04_01实现十分相近。两个重载try_pop()除了在队列为空时抛出异常，
 * 其他的与Ex04_01中pop()函数一样。当指针指向空值的时候会返回NULL指针⑤是实现栈的有效途径。所以即使排除掉wait_and_pop()函数，之前对栈的分析依旧适用。
 *
 * wiat_and_pop()函数是等待队列向栈进行输入的一个解决方案；对于data_cond.wait()的调用，直到队列中有一个元素的时候才会返回，所以不用担心出现一个空队列的情况，
 * 还有数据会一直被互斥锁保护。因为不变量这里并未发生变化，所以函数不会添加新的条件竞争或是死锁的可能。异常安全在这里的会有一些变化，当不止一个线程等待对队列
 * 进行推送操作时只会有一个线程，因得到data_cond.notify_one()而继续工作。但是如果这个工作线程在wait_and_pop()中抛出一个异常，例如：构造新的std::shared_ptr<>
 * 对象④时抛出异常，那么其他线程则会永世长眠。当这种情况是不可接受时，这里的调用就需要改成data_cond.notify_all()，这个函数将唤醒所有的工作线程，不过当大多
 * 线程发现队列依旧是空时，又会耗费很多资源让线程重新进入睡眠状态。
 * 第二种替代方案：当有异常抛出的时候，让wait_and_pop()函数调用notify_one()从而让个另一个线程可以去尝试索引存储的值。
 * 第三种替代方案：将std::shared_ptr<>的初始化过程移到push()中，并且存储std::shared_ptr<>实例，而非直接使用数据的值。将std::shared_ptr<>拷贝到内部
 * std::queue<> 中就不会抛出异常，这样wait_and_pop()又是安全的。
 *
 */
