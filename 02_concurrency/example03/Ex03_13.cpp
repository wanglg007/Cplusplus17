//使用非原子操作执行序列
#include <atomic>
#include <thread>
#include <assert.h>

bool x = false;                 // x现在是一个非原子变量
std::atomic<bool> y;
std::atomic<int> z;

void write_x_then_y() {
    x = true;                   // 1 在栅栏前存储x
    std::atomic_thread_fence(std::memory_order_release);
    y.store(true, std::memory_order_relaxed);       // 2 在栅栏后存储y
}

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed));     // 3 在#2写入前，持续等
    std::atomic_thread_fence(std::memory_order_acquire);
    if (x)                                          // 4 这里读取到的值，是#1中写入
        ++z;
}

int main() {
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x_then_y);
    std::thread b(read_y_then_x);
    a.join();
    b.join();
    assert(z.load() != 0);                          // 5 断言将不会触发
}

/**
 * 栏仍然为存储x①和存储y②，还有加载y③和加载x④提供一个执行序列，并且这里仍然有一个先行关系，在存储x和加载x之间，所以断言⑤不会被触发。②中的存储和③中对y的加载，
 * 都必须是原子操作；否则，将会在y上产生条件竞争，不过一旦读取线程看到存储到y的操作，栅栏将会对x执行有序的操作。这个执行顺序意味着，x上不存在条件竞争，即使它被另外
 * 的线程修改或被其他线程读取。
 *
 */

