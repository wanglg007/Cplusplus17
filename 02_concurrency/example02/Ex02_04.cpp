//提取push()和wait_and_pop()
#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class threadsafe_queue {
private:
    std::mutex mut;
    std::queue<T> data_queue;
    std::condition_variable data_cond;

public:
    void push(T new_value) {
        std::lock_guard<std::mutex> lk(mut);
        data_queue.push(new_value);
        data_cond.notify_one();
    }

    void wait_and_pop(T &value) {
        std::unique_lock<std::mutex> lk(mut);
        data_cond.wait(lk, [this] { return !data_queue.empty(); });
        value = data_queue.front();
        data_queue.pop();
    }
};

struct data_chunk {
};

data_chunk prepare_data();

bool more_data_to_prepare();

void process(data_chunk);

bool is_last_chunk(data_chunk);

threadsafe_queue<data_chunk> data_queue;            // 1

void data_preparation_thread() {
    while (more_data_to_prepare()) {
        data_chunk const data = prepare_data();
        data_queue.push(data);                      // 2
    }
}

void data_processing_thread() {
    while (true) {
        data_chunk data;
        data_queue.wait_and_pop(data);              // 3
        process(data);
        if (is_last_chunk(data))
            break;
    }
}

int main() {

}

/**
 * 线程队列的实例中包含有互斥量和条件变量，所以独立的变量就不需要①，并且调用push()也不需要外部同步②。当然，wait_and_pop()还要兼顾条件变量的等待③。
 */