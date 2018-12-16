//非限制操作只有非常少的顺序要求
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);   // 1
    y.store(true, std::memory_order_relaxed);   // 2
}

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed)); // 3
    if (x.load(std::memory_order_relaxed))      // 4
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
    assert(z.load() != 0);                      // 5
}

/**
 * 这次assert⑤可能会触发，因为加载x的操作④可能读取到false，即使加载y的操作③读取到true，并且存储x的操作①先发与存储y的操作②。x和y是两个不同的变量，
 * 所以这里没有顺序去保证每个操作产生相关值的可见性。
 *
 */