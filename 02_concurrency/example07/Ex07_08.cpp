//使用任务窃取的线程池
class thread_pool {
    typedef function_wrapper task_type;

    std::atomic_bool done;
    thread_safe_queue <task_type> pool_work_queue;
    std::vector <std::unique_ptr<work_stealing_queue>> queues;          // 1
    std::vector <std::thread> threads;
    join_threads joiner;

    static thread_local work_stealing_queue *local_work_queue;      // 2
    static thread_local unsigned my_index;

    void worker_thread(unsigned my_index_) {
        my_index = my_index_;
        local_work_queue = queues[my_index].get();                     // 3
        while (!done) {
            run_pending_task();
        }
    }

    bool pop_task_from_local_queue(task_type &task) {
        return local_work_queue && local_work_queue->try_pop(task);
    }

    bool pop_task_from_pool_queue(task_type &task) {
        return pool_work_queue.try_pop(task);
    }

    bool pop_task_from_other_thread_queue(task_type &task) {        // 4
        for (unsigned i = 0; i < queues.size(); ++i) {
            unsigned const index = (my_index + i + 1) % queues.size();  // 5
            if (queues[index]->try_steal(task)) {
                return true;
            }
        }

        return false;
    }

public:
    thread_pool() :
            joiner(threads), done(false) {
        unsigned const thread_count = std::thread::hardware_concurrency();

        try {
            for (unsigned i = 0; i < thread_count; ++i) {
                queues.push_back(std::unique_ptr<work_stealing_queue>(new work_stealing_queue));    // 6
                threads.push_back(std::thread(&thread_pool::worker_thread, this, i));
            }
        }
        catch (...) {
            done = true;
            throw;
        }
    }

    ~thread_pool() {
        done = true;
    }

    template<typename ResultType>
    using task_handle=std::unique_future<ResultType>;

    template<typename FunctionType>
    task_handle<std::result_of<FunctionType()>::type> submit(FunctionType f) {
        typedef std::result_of<FunctionType()>::type result_type;

        std::packaged_task < result_type() > task(f);
        task_handle<result_type> res(task.get_future());
        if (local_work_queue) {
            local_work_queue->push(std::move(task));
        } else {
            pool_work_queue.push(std::move(task));
        }
        return res;
    }

    void run_pending_task() {
        task_type task;
        if (pop_task_from_local_queue(task) ||              // 7
            pop_task_from_pool_queue(task) ||               // 8
            pop_task_from_other_thread_queue(task)) {       // 9
            task();
        } else {
            std::this_thread::yield();
        }
    }
};

/**
 * This code is very similar to listing Ex06_06. The first difference is that each thread has a work_stealing_queue rather
 * than a plain std::queue<> (2). When each thread is created,rather than allocating its own work queue, the pool constructor
 * allocates one (6),which is then stored in the list of work queues for this pool (1). The index of the queue in the list
 * is then passed in to the thread function and used to retrieve the pointer to the queue (3). This means that the thread
 * pool can access the queue when trying to steal a task for a thread that has no work to do. run_pending_task() will now
 * try to take a task from its thread’s own queue (7), take a task from the pool queue (8), or take a task from the queue
 * of another thread (9).
 *
 * pop_task_from_other_thread_queue() (4) iterates through the queues belonging to all the threads in the pool, trying to
 * steal a task from each in turn. In order to avoid every thread trying to steal from the first thread in the list, each
 * thread starts at the next thread in the list, by offsetting the index of the queue to check by its own index (5).
 *
 */
