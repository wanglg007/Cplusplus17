//run_pending_task()函数实现
void thread_pool::run_pending_task() {
    function_wrapper task;
    if (work_queue.try_pop(task)) {
        task();
    } else {
        std::this_thread::yield();
    }
}

/**
 * This implementation of run_pending_task() is lifted straight out of the main loop of the worker_thread() function, which
 * can now be modified to call the extracted run_pending_task(). This tries to take a task of the queue and run it if there
 * is one; otherwise, it yields to allow the OS to reschedule the thread.
 *
 */