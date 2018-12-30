//并行版std::for_each
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <std::future<void>> futures(num_threads - 1);           // 1
    std::vector <std::thread> threads(num_threads - 1);
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task<void(void)> task(
                [=]() {
                    std::for_each(block_start, block_end, f);
                });                                                     // 2
        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task));                      // 3
        block_start = block_end;
    }
    std::for_each(block_start, last, f);
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        futures[i].get();                                               // 4
    }
}

/**
 * The basic structure of the code is identical to that of listing Ex06_04, which is unsurprising. The key difference is
 * that the futures vector stores std::future<void> (1) because the worker threads don’t return a value, and a simple
 * lambda function that invokes the function f on the range from block_start to block_end is used for the task (2). This
 * avoids having to pass the range into the thread constructor (3). Since the worker threads don’t return a value, the
 * calls to futures[i].get() (4) just provide a means of retrieving any exceptions thrown on the worker threads; if you
 * don’t wish to pass on the exceptions, you could omit this.
 *
 */