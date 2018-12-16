//全序——序列一致
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x() {
    x.store(true, std::memory_order_seq_cst);       // 1
}

void write_y() {
    y.store(true, std::memory_order_seq_cst);       // 2
}

void read_x_then_y() {
    while (!x.load(std::memory_order_seq_cst));
    if (y.load(std::memory_order_seq_cst))          // 3
        ++z;
}

void read_y_then_x() {
    while (!y.load(std::memory_order_seq_cst));
    if (x.load(std::memory_order_seq_cst))          // 4
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

    assert(z.load() != 0);                          // 5
}

/**
 * assert⑤语句是永远不会触发的，因为不是存储x的操作①发生，就是存储y的操作②发生。如果在read_x_then_y中加载y③返回false，那是因为存储x的操作肯定发生在存储y的操作之
 * 前，那么在这种情况下在read_y_then_x中加载x④必定会返回true，因为while循环能保证在某一时刻y是true。因为memory_order_seq_cst的语义需要一个单全序将所有操作都标记为
 * memory_order_seq_cst，这就暗示着“加载y并返回false③”与“存储y①”的操作，有一个确定的顺序。只有一个全序时，如果一个线程看到x==true，随后又看到y==false，这就意
 * 味着在总序列中存储x的操作发生在存储y的操作之前。
 *
 * 当然，因为所有事情都是对称的，所以就有可能以其他方式发生，比如，加载x④的操作返回false，或强制加载y③的操作返回true。在这两种情况下，z都等于1。当两个加载操作都
 * 返回true，z就等于2，所以任何情况下，z都不能是0。
 */