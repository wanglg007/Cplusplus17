//使用获取和释放顺序进行同步传递
#include <atomic>
#include <thread>
#include <assert.h>

std::atomic<int> data[5];
std::atomic<bool> sync1(false), sync2(false);

void thread_1() {
    data[0].store(42, std::memory_order_relaxed);
    data[1].store(97, std::memory_order_relaxed);
    data[2].store(17, std::memory_order_relaxed);
    data[3].store(-141, std::memory_order_relaxed);
    data[4].store(2003, std::memory_order_relaxed);
    sync1.store(true, std::memory_order_release);           // 1.设置sync1
}

void thread_2() {
    while (!sync1.load(std::memory_order_acquire));         // 2.直到sync1设置
    sync2.store(std::memory_order_release);                  // 3.设置sync2
}

void thread_3() {
    while (!sync2.load(std::memory_order_acquire));         // 4.直到sync1设置
    assert(data[0].load(std::memory_order_relaxed) == 42);
    assert(data[1].load(std::memory_order_relaxed) == 97);
    assert(data[2].load(std::memory_order_relaxed) == 17);
    assert(data[3].load(std::memory_order_relaxed) == -141);
    assert(data[4].load(std::memory_order_relaxed) == 2003);
}

int main() {
    std::thread t1(thread_1);
    std::thread t2(thread_2);
    std::thread t3(thread_3);
    t1.join();
    t2.join();
    t3.join();
}

/**
 * 尽管thread_2只接触到变量syn1②和sync2③，不过这对于thread_1和thread_3的同步就足够，这就能保证断言不会触发。首先，thread_1将数据存储到data中先行与
 * 存储sync1①（它们在同一个线程内）。因为加载sync1①的是一个while循环，它最终会看到thread_1存储的值(是从“释放-获取”对的后半对获取)。因此，对于sync1
 * 的存储先行与最终对于sync1的加载(在while循环中)。thread_3的加载操作④，位于存储sync2③操作的前面(也就是先行)。存储sync2③因此先行于thread_3的加载④，
 * 加载又先行与存储sync2③，存储sync2又先行与加载sync2④，加载syn2又先行与加载data。因此，thread_1存储数据到data的操作先行于thread_3中对data的加载，并且
 * 保证断言都不会触发。
 *
 */
