//获取-释放操作会影响序列中的释放操作
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);   // 1 自旋，等待y被设置为true
    y.store(true, std::memory_order_release);   // 2
}

void read_y_then_x() {
    while (!y.load(std::memory_order_acquire)); // 3
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
 * 读取y③时会得到true，和存储时写入的一样②。因为存储使用的是memory_order_release，读取使用的是memory_order_acquire，存储就与读取就同步了。因为
 * 这两个操作是由同一个线程完成的，所以存储x①先行与加载y②。对y的存储同步与对y的加载，存储x也就先行于对y的加载，并且扩展先行与x的读取。因此，加
 * 载x的值必为true，并且断言⑤不会触发。如果对于y的加载不是在while循环中，那么情况可能就会有所不同；加载y的时候可能会读取到false，在这种情况下对
 * 于读取到的x是什么值，就没有要求了。为了保证同步，加载和释放操作必须成对。所以，无论有何影响，释放操作存储的值，必须要让获取操作看到。当存储
 * 如②或加载如③，都是一个释放操作时，对x的访问就无序了，也就无法保证④处读到的是true，并且还会触发断言。
 *
 */
