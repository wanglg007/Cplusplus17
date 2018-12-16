//使用原子操作从队列中读取数据
#include <atomic>
#include <thread>
#include <vector>

std::vector<int> queue_data;
std::atomic<int> count;

void wait_for_more_items() {}

void process(int) {}

void populate_queue() {
    unsigned const number_of_items = 20;
    queue_data.clear();
    for (unsigned i = 0; i < number_of_items; ++i) {
        queue_data.push_back(i);
    }

    count.store(number_of_items, std::memory_order_release);        // 1 初始
}

void consume_queue_items() {
    while (true) {
        int item_index;
        if ((item_index = count.fetch_sub(1, std::memory_order_acquire)) <= 0) {
            wait_for_more_items();                                  // 3 等待更多元素
            continue;
        }
        process(queue_data[item_index - 1]);                        // 4 安全读取queue_data
    }
}

int main() {
    std::thread a(populate_queue);
    std::thread b(consume_queue_items);
    std::thread c(consume_queue_items);
    a.join();
    b.join();
    c.join();
}

/**
 * 一种处理方式是让线程产生数据，并存储到一个共享缓存中，而后调用count.store(number_of_items, memory_order_release)①让其他线程知道数据是可用的。线
 * 程群消耗着队列中的元素，之后可能调用count.fetch_sub(1, memory_order_acquire)②向队列索取一个元素，不过在这之前，需要对共享缓存进行完整的读取④。
 * 一旦count归零，那么队列中就没有更多的元素了，当元素耗尽时线程必须等待③。
 *
 *
 */

