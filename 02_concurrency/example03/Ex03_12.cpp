//栅栏可以让自由操作变的有序
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<bool> x, y;
std::atomic<int> z;

void write_x_then_y() {
    x.store(true, std::memory_order_relaxed);               // 1
    std::atomic_thread_fence(std::memory_order_release);     // 2
    y.store(true, std::memory_order_relaxed);               // 3
}

void read_y_then_x() {
    while (!y.load(std::memory_order_relaxed));             // 4
    std::atomic_thread_fence(std::memory_order_acquire);     // 5
    if (x.load(std::memory_order_relaxed))                   // 6
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
    assert(z.load() != 0);                                    // 7
}

/**
 * 释放栅栏②与获取栅栏⑤同步，这是因为加载y的操作④读取的是在③处存储的值。所以，在①处存储x先行与⑥处加载x，最后x读取出来必为true，并且断言不会
 * 被触发⑦。原先不带栅栏的存储和加载x都是无序的，并且断言是可能会触发的。需要注意的是，这两个栅栏都是必要 的：你需要在一个线程中进行释放，然后在
 * 另一个线程中进行获取，这样才能构建出同步关系。
 *
 * 在这个例子中，如果存储y的操作③标记为memory_order_release，而非memory_order_relaxed的话，释放栅栏②也会对这个操作产生影响。同样的，当加载y的操作
 * ④标记为memory_order_acquire时，获取栅栏⑤也会对之产生影响。使用栅栏的一般想法是：当一个获取操作能看到释放栅栏操作后的存储结果，那么这个栅栏就与
 * 获取操作同步；并且，当加载操作在获取栅栏操作前，看到一个释放操作的结果，那么这个释放操作同步于获取栅栏。当然，你也可以使用双边栅栏操作，举一个简
 * 单的例子，当一个加载操作在获取栅栏前，看到一个值有存储操作写入，且这个存储操作发生在释放栅栏后，那么释放栅栏与获取栅栏是同步的。
 *
 * 虽然，栅栏同步依赖于读取/写入的操作发生于栅栏之前/后，但是这里有一点很重要：同步点，就是栅栏本身。当你执行清单中的write_x_then_y，并且在栅栏操作
 * 之后对x进行写入，就像下面的代码一样。这里，触发断言的条件就不保证一定为true了，尽管写入x的操作在写入y的操作之前发生。
 *   void write_x_then_y()
 *   {
 *   std::atomic_thread_fence(std::memory_order_release);
 *   x.store(true,std::memory_order_relaxed);
 *   y.store(true,std::memory_order_relaxed);
 *   }
 *  这里里的两个操作，就不会被栅栏分开，并且也不再有序。只有当栅栏出现在存储x和存储y操作之间，这个顺序是硬性的。当然，栅栏是否存在不会影响任何拥有先
 *  行关系的执行序列，这种情况是因为一些其他原子操作。
 *
 */