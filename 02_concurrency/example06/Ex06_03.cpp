//使用 std::packaged_task 的并行 std::accumulate
template<typename Iterator, typename T>
struct accumulate_block {
    T operator()(Iterator first, Iterator last) {       // 1
        return std::accumulate(first, last, T());       // 2
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <std::future<T>> futures(num_threads - 1);                                  // 3
    std::vector <std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task < T(Iterator, Iterator) > task(accumulate_block<Iterator, T>()); // 4
        futures[i] = task.get_future();                                                     // 5
        threads[i] = std::thread(std::move(task), block_start, block_end);                  // 6
        block_start = block_end;
    }
    T last_result = accumulate_block()(block_start, last);                                  // 7

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));

    T result = init;                                                                        // 8
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        result += futures[i].get();                                                         // 9
    }
    result += last_result;                                                                  // 10
    return result;
}

/**
 * The first change is that the function call operator of accumulate_block now returns the result directly, rather than
 * taking a reference to somewhere to store it (1). You’re using std::packaged_task and std::future for the exception
 * safety, so you can use it to transfer the result too. This does require that you explicitly pass in a default
 * constructed T in the call to std::accumulate (2) rather than reusing the supplied result value, but that’s a minor change.
 *
 * The next change is that rather than having a vector of results, you have a vector of futures (3) to store a std::future<T>
 * for each spawned thread. In the thread-spawning loop, you first create a task for accumulate_block (4). std::packaged_task<T(Iterator,Iterator)>
 * declares a task that takes two Iterators and returns a T, which is what your function does. You then get the future for that task (5) and run
 * that task on a new thread, passing in the start and end of the block to process (5). When the task runs,the result will be captured in the future,
 * as will any exception thrown.
 *
 * Since you’ve been using futures, you don’t have a result array, so you must store the result from the final block in a variable (7) rather
 * than in a slot in the array. Also,because you have to get the values out of the futures, it’s now simpler to use a basic for loop rather
 * than std::accumulate, starting with the supplied initial value (8) and adding in the result from each future (9). If the corresponding task
 * threw an exception, this will have been captured in the future and will now be thrown again by the call to get(). Finally, you add the result
 * from the last block (10) before returning the overall result to the caller.
 *
 *  So, that’s removed one of the potential problems: exceptions thrown in the worker threads are rethrown in the main thread.
 *  If more than one of the worker threads throws an exception, only one will be propagated, but that’s not too big a deal. If
 *  it really matters, you can use something like std::nested_exception to capture all the exceptions and throw that instead.
 *
 *  All the threads will be joined, no matter how the code leaves the block. However, try-catch blocks are ugly, and you have
 *  duplicate code. You’re joining the threads both in the “normal” control flow and in the catch block. Duplicate code
 *  is rarely a good thing, because it means more places to change. Instead, let’s extract this out into the destructor of
 *  an object; it is, after all, the idiomatic way of cleaning up resources in C++. Here’s your class:
 *  class join_threads
 *  {
 *   std::vector<std::thread>& threads;
 *   public:
 *    explicit join_threads(std::vector<std::thread>& threads_):
 *    threads(threads_)
 *    {}
 *    ~join_threads()
 *    {
 *    for(unsigned long i=0;i<threads.size();++i)
 *    {
 *    if(threads[i].joinable())
 *    threads[i].join();
 *    }
 *    }
 *   };
 *
 */