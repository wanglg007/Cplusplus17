//使用栈的并行快速排序算法——等待数据块排序
template<typename T>
struct sorter                                       // 1
{
    struct chunk_to_sort {
        std::list <T> data;
        std::promise <std::list<T>> promise;
    };

    thread_safe_stack <chunk_to_sort> chunks;        // 2
    std::vector <std::thread> threads;               // 3
    unsigned const max_thread_count;
    std::atomic<bool> end_of_data;

    sorter() :
            max_thread_count(std::thread::hardware_concurrency() - 1),
            end_of_data(false) {}

    ~sorter()           // 4
    {
        end_of_data = true;       // 5
        for (unsigned i = 0; i < threads.size(); ++i) {
            threads[i].join();  // 6
        }
    }

    void try_sort_chunk() {
        boost::shared_ptr <chunk_to_sort> chunk = chunks.pop();   // 7
        if (chunk) {
            sort_chunk(chunk);  // 8
        }
    }

    std::list <T> do_sort(std::list <T> &chunk_data)  // 9
    {
        if (chunk_data.empty()) {
            return chunk_data;
        }

        std::list <T> result;
        result.splice(result.begin(), chunk_data, chunk_data.begin());
        T const &partition_val = *result.begin();

        typename std::list<T>::iterator divide_point =
                std::partition(chunk_data.begin(), chunk_data.end(),
                               [&](T const &val) { return val < partition_val; });    // 10
        chunk_to_sort new_lower_chunk;
        new_lower_chunk.data.splice(new_lower_chunk.data.end(),
                                    chunk_data, chunk_data.begin(),
                                    divide_point);

        std::future <std::list<T>> new_lower =
                new_lower_chunk.promise.get_future();
        chunks.push(std::move(new_lower_chunk));    // 11
        if (threads.size() < max_thread_count) {    // 12
            threads.push_back(std::thread(&sorter<T>::sort_thread, this));
        }

        std::list <T> new_higher(do_sort(chunk_data));

        result.splice(result.end(), new_higher);
        while (new_lower.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {  // 13
            try_sort_chunk();       // 14
        }

        result.splice(result.begin(), new_lower.get());
        return result;
    }

    void sort_chunk(boost::shared_ptr <chunk_to_sort> const &chunk) {
        chunk->promise.set_value(do_sort(chunk->data));
    }

    void sort_thread() {
        while (!end_of_data) {  // 15
            try_sort_chunk();   // 17
            std::this_thread::yield();  // 18
        }
    }
};

template<typename T>
std::list <T> parallel_quick_sort(std::list <T> input) {    // 19
    if (input.empty()) {
        return input;
    }
    sorter<T> s;
    return s.do_sort(input);        // 20
}

/**
 * 这里，parallel_quick_sort函数⑲代表了sorter类①的功能，其支持在栈上简单的存储无序数据
块②，并且对线程进行设置③。do_sort成员函数⑨主要做的就是对数据进行划分⑩。相较于对
每一个数据块产生一个新的线程，这次会将这些数据块推到栈上⑪；并在有备用处理器⑫的
时候，产生新线程。因为小于部分的数据块可能由其他线程进行处理，那么就得等待这个线
程完成⑬。为了让所有事情顺利进行(只有一个线程和其他所有线程都忙碌时)，当线程处于等
待状态时⑭，就让当前线程尝试处理栈上的数据。try_sort_chunk只是从栈上弹出一个数据块
⑦，并且对其进行排序⑧，将结果存在promise中，让线程对已经存在于栈上的数据块进行提
取⑮。
 * 当end_of_data没有被设置时⑯，新生成的线程还在尝试从栈上获取需要排序的数据块⑰。在
循环检查中，也要给其他线程机会⑱，可以从栈上取下数据块进行更多的操作。这里的实现
依赖于sorter类④对线程的清理。当所有数据都已经排序完成，do_sort将会返回(即使还有工
作线程在运行)，所以主线程将会从parallel_quick_sort⑳中返回，在这之后会销毁sorter对
象。析构函数会设置end_of_data标志⑤，以及等待所有线程完成工作⑥。标志的设置将终止
线程函数内部的循环⑯。
 *
 *在这个方案中，不用为spawn_task产生的无数线程所困扰，并且也不用再依赖C++线程库，
为你选择执行线程的数量(就像 std::async() 那样)。该方案制约线程数量的值就
是 std::thread::hardware_concurrency() 的值，这样就能避免任务过于频繁的切换。不过，这
里还有两个问题：线程管理和线程通讯。要解决这两个问题就要增加代码的复杂程度。虽
然，线程对数据项是分开处理的，不过所有对栈的访问，都可以向栈添加新的数据块，并且
移出数据块以作处理。这里重度的竞争会降低性能(即使使用无锁(无阻塞)栈)，原因将会在后
面提到。
 *
 *这个方案使用到了一个特殊的线程池——所有线程的任务都来源于一个等待链表，然后线程
会去完成任务，完成任务后会再来链表提取任务。
 *
 *
 */