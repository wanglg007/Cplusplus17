//异常安全版 std::accumulate
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads =
            (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads =
            std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <std::future<T>> futures(num_threads - 1);
    std::vector <std::thread> threads(num_threads - 1);
    join_threads joiner(threads);   // 1

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task < T(Iterator, Iterator) > task(
                accumulate_block<Iterator, T>());
        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task), block_start, block_end);
        block_start = block_end;
    }
    T last_result = accumulate_block()(block_start, last);
    T result = init;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        result += futures[i].get();// 2
    }
    result += last_result;
    return result;
}

/**
 * 当创建了线程容器，就对新类型创建了一个实例①，可让退出线程进行汇入。然后，可以再显
式的汇入循环中将线程删除，在原理上来说是安全的：因为线程，无论怎么样退出，都需要
汇入主线程。注意这里对futures[i].get()②的调用，将会阻塞线程，直到结果准备就绪，所以
这里不需要显式的将线程进行汇入。和清单8.2中的原始代码不同：原始代码中，你需要将线
程汇入，以确保results向量被正确填充。不仅需要异常安全的代码，还需要较短的函数实
现，因为这里已经将汇入部分的代码放到新(可复用)类型中去了。
 *
 *
 */