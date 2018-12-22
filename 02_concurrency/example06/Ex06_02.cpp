//std::accumulate 的原始并行版本
template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T &result) {
        result = std::accumulate(first, last, result);              // 1
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);    // 2

    if (!length)
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <T> results(num_threads);               // 3
    std::vector <std::thread> threads(num_threads - 1); // 4

    Iterator block_start = first;                       // 5
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;               // 6
        std::advance(block_end, block_size);
        threads[i] = std::thread(
                accumulate_block<Iterator, T>(),
                block_start, block_end, std::ref(results[i]));// 7
        block_start = block_end;                        // 8
    }
    accumulate_block()(block_start, last, results[num_threads - 1]);    // 9

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    return std::accumulate(results.begin(), results.end(), init);       // 10
}

/**
 * 首先，需要调用distance②，其会对用户定义的迭代器类型进行操作。因为，这时还没有做任
何事情，所以对于调用线程来说，所有事情都没问题。接下来，就需要分配results③和
threads④。再后，调用线程依旧没有做任何事情，或产生新的线程，所以到这里也是没有问
 题的。当然，如果在构造threads抛出异常，那么对已经分配的results将会被清理，析构函数
会帮你打理好一切。
 *
 * 跳过block_start⑤的初始化(因为也是安全的)，来到了产生新线程的循环⑥⑦⑧。当在⑦处创
建了第一个线程，如果再抛出异常，就会出问题的；对于新的 std::thread 对象将会销毁，程
序将调用 std::terminate 来中断程序的运行。使用 std::terminate 的地方，可不是什么好地
方。
 *
 * accumulate_block⑨的调用就可能抛出异常，就会产生和上面类似的结果；线程对象将会被销
毁，并且调用 std::terminate 。另一方面，最终调用 std::accumulate ⑩可能会抛出异常，不
过处理起来没什么难度，因为所有的线程在这里已经汇聚回主线程了。

 *
 * 上面只是对于主线程来说的，不过还有很多地方会抛出异常：对于调用accumulate_block的
新线程来说就会抛出异常①。没有任何catch块，所以这个异常不会被处理，并且当异常发生
的时候会调用 std::terminater() 来终止应用的运行。
也许这里的异常问题并不明显，不过这段代码是非异常安全的
 *
 */