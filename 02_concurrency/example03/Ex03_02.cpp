//不同线程对数据的读写
#include <vector>
#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>

std::vector<int> data;
std::atomic_bool data_ready(false);

void reader_thread() {
    while (!data_ready.load()) {                                    // 1
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << "The answer=" << data[0] << "\n";                  // 2
}

void writer_thread() {
    data.push_back(42);                                             // 3
    data_ready = true;                                             // 4
}

int main() {

}

/**
 * 把等待数据的低效循环①放在一边（需要这个循环，否则想要在线程间共享数据就是不切实际的：数据的每一项都必须是原子的）。当非原子读②和写③对同一数据结构
 * 进行无序访问时，将会导致未定义行为的发生，因此这个循环就是确保访问循序被严格的遵守的。
 *
 * 强制访问顺序是由对std::atomic<bool>类型的data_ready变量进行操作完成的；这些操作通过“先行发生”(happens-before)和“同步发生”(synchronizes-with)确定
 * 必要的顺序。写入数据③的操作，在写入data_ready标志④的操作前发生，并且读取标志①发生在读取数据②之前。当data_ready①为true，写操作就会与读操作同步，
 * 建立一个“先行发生”关系。因为“先行发生”是可传递的，所以读取数据③先行与写入标志④，这两个行为有先行与读取标志的操作①，之前的操作都先行与读取数据②，
 * 这样你就拥有了强制顺序：写入数据先行与读取数据，其他没问题。
 *
 */