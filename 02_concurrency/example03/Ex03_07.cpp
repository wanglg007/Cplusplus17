//获取-释放不意味着统一操作顺序
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
    x.store(true, std::memory_order_release);
}

void write_y() {
    y.store(true, std::memory_order_release);
}

void read_x_then_y() {
    while (!x.load(std::memory_order_acquire));
    if (y.load(std::memory_order_acquire))          // 1
        ++z;
}

void read_y_then_x() {
    while (!y.load(std::memory_order_acquire));
    if (x.load(std::memory_order_acquire))          // 2
        ++z;
}

int main() {
    x = false;
    y = false;
    z = 0;
    std::thread a(write_x);
    std::thread b(write_y);
    std::thread c(read_x_then_y);
    std::thread d(read_y_then_x);
    a.join();
    b.join();
    c.join();
    d.join();
    assert(z.load() != 0);                          // 3
}

/**
 * 例子中断言③可能会触发(就如同自由排序那样)，因为可能在加载x②和y③的时候，读取到的是false。因为x和y是由不同线程写入，所以序列中的每一次释放到获取都
 * 不会影响到其他线程的操作。
 *
 */