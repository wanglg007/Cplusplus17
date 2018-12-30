//异常安全版 std::accumulate
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <std::future<T>> futures(num_threads - 1);
    std::vector <std::thread> threads(num_threads - 1);
    join_threads joiner(threads);                                   // 1

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task < T(Iterator, Iterator) > task(accumulate_block<Iterator, T>());
        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task), block_start, block_end);
        block_start = block_end;
    }
    T last_result = accumulate_block()(block_start, last);
    T result = init;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        result += futures[i].get();                                 // 2
    }
    result += last_result;
    return result;
}

/**
 * Once you’ve created your container of threads, you create an instance of your new class (1) to join with all the threads
 * on exit. You can then remove your explicit join loop, safe in the knowledge that the threads will be joined however the
 * function exits.Note that the calls to futures[i].get() (2) will block until the results are ready, so you don’t need to have
 * explicitly joined with the threads at this point. This is unlike the original from listing Ex06_02, where you needed to have
 * joined with the threads to ensure that the results vector was correctly populated. Not only do you get exceptionsafe code,
 * but your function is actually shorter because you’ve extracted the join code into your new (reusable) class.
 *
 *
 */