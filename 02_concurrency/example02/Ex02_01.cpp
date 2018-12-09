//(2.1)使用std::condition_variable处理数据等待
#include <mutex>
#include <condition_variable>
#include <thread>
#include <queue>

bool more_data_to_prepare() {
    return false;
}

struct data_chunk {
};

data_chunk prepare_data() {
    return data_chunk();
}

void process(data_chunk &) {
}

bool is_last_chunk(data_chunk &) {
    return true;
}

std::mutex mut;
std::queue<data_chunk> data_queue;          // 1
std::condition_variable data_cond;

void data_preparation_thread() {
    while (more_data_to_prepare()) {
        data_chunk const data = prepare_data();
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(data);              // 2
        data_cond.notify_one();             // 3
    }
}

void data_processing_thread() {
    while (true) {
        std::unique_lock<std::mutex> lk(mut);                       // 4
        data_cond.wait(lk, [] { return !data_queue.empty(); });    // 5
        data_chunk data = data_queue.front();
        data_queue.pop();
        lk.unlock();                                                // 6
        process(data);
        if (is_last_chunk(data))
            break;
    }
}

int main() {
    std::thread t1(data_preparation_thread);
    std::thread t2(data_processing_thread);

    t1.join();
    t2.join();
}

/**
 * 首先拥有一个用来在两个线程之间传递数据的队列①。当数据准备好时，使用std::lock_guard对队列上锁，将准备好的数据压入队列中②，之后线程会对队列中的数据上锁。
 * 然后调用std::condition_variable的notify_one()成员函数，对等待的线程(如果有等待线程)进行通知③。
 *
 * 在另外一侧有个正在处理数据的线程，该线程首先对互斥量上锁，但在这里std::unique_lock要比std::lock_guard ④更加合适。线程之后会调用std::condition_variable
 * 的成员函数wait()，传递一个锁和一个lambda函数表达式(作为等待的条件⑤)。Lambda函数会去检查data_queue是否不为空，当data_queue不为空，则意味着队列中已经准备好数据。
 *
 * wait()会去检查这些条件，当条件满足(lambda函数返回true)时返回。如果条件不满足，wait()函数将解锁互斥量，并且将这个线程置于阻塞或等待状态。
 * 当准备数据的线程调用notify_one()通知条件变量时，处理数据的线程从睡眠状态中苏醒，重新获取互斥锁，并且对条件再次检查，在条件满足的情况下，从wait()返回并继续持
 * 有锁。当条件不满足时，线程将对互斥量解锁，并且重新开始等待。这就是为什么用std::unique_lock而不使用std::lock_guard。
 * 等待中的线程必须在等待期间解锁互斥量，并在这这之后对互斥量再次上锁，而std::lock_guard没有这么灵活。如果互斥量在线程休眠期间保持锁住状态，准备数据的线程将无法
 * 锁住互斥量，也无法添加数据到队列中；同样的，等待线程也永远不会知道条件何时满足。
 *
 * 使用lambda函数用于等待⑤，这个函数用于检查队列何时不为空，不过任意的函数和可调用对象都可以传入wait()。在调用wait()的过程中，一个条件变量可能会去检查给定条件若干次；
 * 然而它总是在互斥量被锁定时这样做，当且仅当提供测试条件的函数返回true时，它就会立即返回。当等待线程重新获取互斥量并检查条件时，如果它并非直接响应另一个线程的通
 * 知，这就是所谓的“伪唤醒”(spurious wakeup)。因为任何伪唤醒的数量和频率都是不确定的，这里不建议使用一个有副作用的函数做条件检查。
 *
 * std::unique_lock不仅适用于对wait()的调用，它还可以用于有待处理但还未处理的数据⑥。处理数据可能是一个耗时的操作。使用队列在多个线程中转移数据是很常见的。
 */