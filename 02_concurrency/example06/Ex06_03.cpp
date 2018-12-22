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
    unsigned long const max_threads =
            (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads =
            std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::vector <std::future<T>> futures(num_threads - 1);      // 3
    std::vector <std::thread> threads(num_threads - 1);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);
        std::packaged_task < T(Iterator, Iterator) > task(accumulate_block<Iterator, T>()); // 4
        futures[i] = task.get_future(); // 5
        threads[i] = std::thread(std::move(task), block_start, block_end);
        block_start = block_end;
    }
    T last_result = accumulate_block()(block_start, last);  // 7

    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    T result = init;    // 8
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        result += futures[i].get(); // 9
    }
    result += last_result;  // 10
    return result;
}

/**
 * 第一个修改就是调用accumulate_block的操作现在就是直接将结果返回，而非使用引用将结
果存储在某个地方①。使用 std::packaged_task 和 std::future 是线程安全的，所以你可以使
用它们来对结果进行转移。当调用 std::accumulate ②时，需要你显示传入T的默认构造函
数，而非复用result的值，不过这只是一个小改动。

 *
 * 下一个改动就是，不用向量来存储结果，而使用futures向量为每个新生线程存
储 std::future<T> ③。在新线程生成循环中，首先要为accumulate_block创建一个任务
④。 std::packaged_task<T(Iterator,Iterator)> 声明，需要操作的两个Iterators和一个想要获
取的T。然后，从任务中获取future⑤，再将需要处理的数据块的开始和结束信息传入⑥，让
新线程去执行这个任务。当任务执行时，future将会获取对应的结果，以及任何抛出的异常。
使用future，就不能获得到一组结果数组，所以需要将最终数据块的结果赋给一个变量进行保
存⑦，而非对一个数组进行填槽。同样，因为需要从future中获取结果，使用简单的for循环，
就要比使用 std::accumulate 好的多；循环从提供的初始值开始⑧，并且将每个future上的值
进行累加⑨。如果相关任务抛出一个异常，那么异常就会被future捕捉到，并且使用get()的时
候获取数据时，这个异常会再次抛出。最后，在返回结果给调用者之前，将最后一个数据块
上的结果添加入结果中⑩
 *
 *这样，一个问题就已经解决：在工作线程上抛出的异常，可以在主线程上抛出。如果不止一
个工作线程抛出异常，那么只有一个能在主线程中抛出，不过这不会有产生太大的问题。如
果这个问题很重要，你可以使用类似 std::nested_exception 来对所有抛出的异常进行捕捉。
 *
 * 剩下的问题就是，当生成第一个新线程和当所有线程都汇入主线程时，抛出异常；这样会让
线程产生泄露。最简单的方法就是捕获所有抛出的线程，汇入的线程依旧是joinable()的，并
且会再次抛出异常：
 try
{
for(unsigned long i=0;i<(num_threads-1);++i)
{
// ... as before
}
T last_result=accumulate_block()(block_start,last);
std::for_each(threads.begin(),threads.end(),
std::mem_fn(&std::thread::join));
}
catch(...)
{
for(unsigned long i=0;i<(num_thread-1);++i)
{
if(threads[i].joinable())
thread[i].join();
}
throw;
}
 *无论线程如何离开这段代码，所有线程都可以被汇入。不过，try-catch很不美观，
并且这里有重复代码。可以将“正常”控制流上的线程在catch块上执行的线程进行汇入。重复
代码是没有必要的，因为这就意味着更多的地方需要改变。不过，现在让我们来提取一个对
象的析构函数；毕竟，析构函数是C++中处理资源的惯用方式。

 class join_threads
{
std::vector<std::thread>& threads;
public:
explicit join_threads(std::vector<std::thread>& threads_):
threads(threads_)
{}
~join_threads()
{
for(unsigned long i=0;i<threads.size();++i)
{
if(threads[i].joinable())
threads[i].join();
}
}
};
 *
 */