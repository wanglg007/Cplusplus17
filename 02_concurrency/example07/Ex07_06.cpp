//线程池——线程具有本地任务队列
class thread_pool {
    thread_safe_queue <function_wrapper> pool_work_queue;

    typedef std::queue <function_wrapper> local_queue_type;                       // 1
    static thread_local std::unique_ptr <local_queue_type> local_work_queue;    // 2

    void worker_thread() {
        local_work_queue.reset(new local_queue_type);                              // 3

        while (!done) {
            run_pending_task();
        }
    }

public:
    template<typename FunctionType>
    std::future <std::result_of<FunctionType()>::type>
    submit(FunctionType f) {
        typedef std::result_of<FunctionType()>::type result_type;

        std::packaged_task < result_type() > task(f);
        std::future <result_type> res(task.get_future());
        if (local_work_queue) {                                                     // 4
            local_work_queue->push(std::move(task));
        } else {
            pool_work_queue.push(std::move(task));                                  // 5
        }
        return res;
    }

    void run_pending_task() {
        function_wrapper task;
        if (local_work_queue && !local_work_queue->empty()) {                       // 6
            task = std::move(local_work_queue->front());
            local_work_queue->pop();
            task();
        } else if (pool_work_queue.try_pop(task)) {                                 // 7
            task();
        } else {
            std::this_thread::yield();
        }
    }
    // rest as before
};

/**
 * We’ve used a std::unique_ptr<> to hold the thread-local work queue (2) because we don’t want non-pool threads to
 * have one; this is initialized in the worker_thread() function before the processing loop (3). The destructor of
 * std::unique_ptr<> will ensure that the work queue is destroyed when the thread exits.
 *
 * submit() then checks to see if the current thread has a work queue (4). If it does,it’s a pool thread, and you can
 * put the task on the local queue; otherwise, you need to put the task on the pool queue as before (5).
 *
 * There’s a similar check in run_pending_task() (6), except this time you also need to check to see if there are any
 * items on the local queue. If there are, you can take the front one and process it; notice that the local queue can be
 * a plain std::queue<> (1) because it’s only ever accessed by the one thread. If there are no tasks on the local queue,
 * you try the pool queue as before (7).
 *
 */

