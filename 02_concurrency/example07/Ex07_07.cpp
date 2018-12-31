//基于锁的任务窃取队列
class work_stealing_queue {
private:
    typedef function_wrapper data_type;
    std::deque <data_type> the_queue;                   // 1
    mutable std::mutex the_mutex;

public:
    work_stealing_queue() {}

    work_stealing_queue(const work_stealing_queue &other) = delete;

    work_stealing_queue &operator=(const work_stealing_queue &other) = delete;

    void push(data_type data) {                        // 2
        std::lock_guard <std::mutex> lock(the_mutex);
        the_queue.push_front(std::move(data));
    }

    bool empty() const {
        std::lock_guard <std::mutex> lock(the_mutex);
        return the_queue.empty();
    }

    bool try_pop(data_type &res) {                     // 3
        std::lock_guard <std::mutex> lock(the_mutex);
        if (the_queue.empty()) {
            return false;
        }

        res = std::move(the_queue.front());
        the_queue.pop_front();
        return true;
    }

    bool try_steal(data_type &res) {                  // 4
        std::lock_guard <std::mutex> lock(the_mutex);
        if (the_queue.empty()) {
            return false;
        }

        res = std::move(the_queue.back());
        the_queue.pop_back();
        return true;
    }
};

/**
 * This queue is a simple wrapper around a std::deque<function_wrapper> (1) that protects all accesses with a mutex lock.
 * Both push() (2) and try_pop() (3) work on the front of the queue, while try_steal() (4) works on the back.
 *
 */

