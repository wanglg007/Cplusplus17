//并行版 std::for_each
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads =
            (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads =
            std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <std::future<void>> futures(num_threads - 1);       // 1
    std::vector <std::thread> threads(num_threads - 1);
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task<void(void)> task(
                [=]() {
                    std::for_each(block_start, block_end, f);
                });     // 2
        futures[i] = task.get_future();
        threads[i] = std::thread(std::move(task));  // 3
        block_start = block_end;
    }
    std::for_each(block_start, last, f);
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        futures[i].get();   // 4
    }
}

/**
 *代码结构与清单8.4的差不多。最重要的不同在于futures向量对 std::future<void> 类型①变量
进行存储，因为工作线程不会返回值，并且简单的lambda函数会对block_start到block_end上
的任务②执行f函数。这是为了避免传入线程的构造函数③。当工作线程不需要返回一个值
时，调用futures[i].get()④只是提供检索工作线程异常的方法；如果不想把异常传递出去，就
可以省略这一步。
 *
 *
 */