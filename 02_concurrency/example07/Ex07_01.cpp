//简单的线程池
class thread_pool {
    std::atomic_bool done;
    thread_safe_queue <std::function<void()>> work_queue;      // 1
    std::vector <std::thread> threads;                          // 2
    join_threads joiner;                                        // 3

    void worker_thread() {
        while (!done) {                                        // 4
            std::function<void()> task;
            if (work_queue.try_pop(task)) {                    // 5
                task();                                         // 6
            } else {
                std::this_thread::yield();                      // 7
            }
        }
    }

public:
    thread_pool() :
            done(false), joiner(threads) {
        unsigned const thread_count = std::thread::hardware_concurrency();         // 8
        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                threads.push_back(std::thread(&thread_pool::worker_thread, this));  // 9
            }
        }
        catch (...) {
            done = true;                                        // 10
            throw;
        }
    }

    ~thread_pool() {
        done = true;                                            // 11
    }

    template<typename FunctionType>
    void submit(FunctionType f) {
        work_queue.push(std::function<void()>(f));              // 12
    }
};

/**
 * This implementation has a vector of worker threads (2) and uses one of the thread-safe queues (1) to manage the queue of work.
 * In this case, users can’t wait for the tasks, and they can’t return any values,so you can use std::function<void()> to
 * encapsulate your tasks. The submit() function then wraps whatever function or callable object is supplied inside a
 * std::function<void()> instance and pushes it on the queue (12).
 *
 * The threads are started in the constructor: you use std::thread::hardware_concurrency() to tell you how many concurrent
 * threads the hardware can support (8),and you create that many threads running your  worker_thread() member function (9).
 *
 *  Starting a thread can fail by throwing an exception, so you need to ensure that any threads you’ve already started are
 *  stopped and cleaned up nicely in this case. This is achieved with a try-catch block that sets the done flag when an
 *  exception is thrown (10),alongside an instance of the join_threads class (3) to join all the threads. This also works
 *  with the destructor: you can just set the done flag (11), and the join_threads instance will ensure that all the threads
 *  have completed before the pool is destroyed. Note that the order of declaration of the members is important:both the
 *  done flag and the worker_queue must be declared before the threads vector,which must in turn be declared before the
 *  joiner. This ensures that the members are destroyed in the right order; you can’t destroy the queue safely until all
 *  the threads have stopped, for example.
 *
 *  The worker_thread function itself is quite simple: it sits in a loop waiting until the done flag is set (4), pulling
 *  tasks off the queue (5) and executing them (6) in the meantime. If there are no tasks on the queue, the function calls
 *  std::this_thread::yield() to take a small break (7) and give another thread a chance to put some work on the queue
 *  before it tries to take some off again the next time around.
 */
