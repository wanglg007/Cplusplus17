//基于线程池的快速排序实现
template<typename T>
struct sorter {                                                 // 1
    thread_pool pool;                                            // 2

    std::list <T> do_sort(std::list <T> &chunk_data) {
        if (chunk_data.empty()) {
            return chunk_data;
        }

        std::list <T> result;
        result.splice(result.begin(), chunk_data, chunk_data.begin());
        T const &partition_val = *result.begin();

        typename std::list<T>::iterator divide_point =
                std::partition(
                        chunk_data.begin(), chunk_data.end(),
                        [&](T const &val) { return val < partition_val; });

        std::list <T> new_lower_chunk;
        new_lower_chunk.splice(new_lower_chunk.end(), chunk_data, chunk_data.begin(), divide_point);

        thread_pool::task_handle <std::list<T>> new_lower =                     // 3
                pool.submit(
                        std::bind(
                                &sorter::do_sort, this,
                                std::move(new_lower_chunk)));

        std::list <T> new_higher(do_sort(chunk_data));

        result.splice(result.end(), new_higher);
        while (!new_lower.is_ready()) {
            pool.run_pending_task();                                            // 4
        }

        result.splice(result.begin(), new_lower.get());
        return result;
    }
};


template<typename T>
std::list <T> parallel_quick_sort(std::list <T> input) {
    if (input.empty()) {
        return input;
    }
    sorter<T> s;

    return s.do_sort(input);
}

/**
 * Just as in listing Ex05_01, you’ve delegated the real work to the do_sort() member function of the sorter class template (1),
 * although in this case the class is only there to wrap the thread_pool instance (2).
 *
 * Your thread and task management is now reduced to submitting a task to the pool (3) and running pending tasks while waiting (4).
 * This is much simpler than in listing Ex05_01,where you had to explicitly manage the threads and the stack of chunks to sort.
 * When submitting the task to the pool, you use std::bind() to bind the this pointer to do_sort() and to supply the chunk to
 * sort. In this case, you call std::move() on the new_lower_chunk as you pass it in, to ensure that the data is moved rather
 * than copied. Although this has now addressed the crucial deadlock-causing problem with tasks hat wait for other tasks, this
 * thread pool is still far from ideal. For starters, every call to submit() and every call to run_pending_task()accesses the
 * same queue.
 *
 */
