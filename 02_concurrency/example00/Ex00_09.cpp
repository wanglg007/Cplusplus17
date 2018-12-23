//(0.9)原生并行版的 std::accumulate
#include <thread>
#include <numeric>
#include <algorithm>
#include <functional>
#include <vector>
#include <iostream>

template<typename Iterator, typename T>
struct accumulate_block {
    void operator()(Iterator first, Iterator last, T &result) {
        result = std::accumulate(first, last, result);
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);

    if (!length)                                                                                                        // 1
        return init;

    unsigned long const min_per_thread = 25;
    unsigned long const max_thread = (length + min_per_thread - 1) / min_per_thread;                                 // 2

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_thread);            // 3

    unsigned long const block_size = length / num_threads;                                                           // 4

    std::vector<T> results(num_threads);
    std::vector<std::thread> threads(num_threads - 1);                                                                  // 5

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_end = block_start;
        std::advance(block_end, block_size);                                                                            // 6
        threads[i] = std::thread(accumulate_block<Iterator, T>(), block_start, block_end, std::ref(results[i]));        // 7
        block_start = block_end;                                                                                        // 8
    }
    accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);                                       // 9

    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));                                     // 10

    return std::accumulate(results.begin(), results.end(), init);                                                      // 11
}

int main() {
    std::vector<int> vi;
    for (int i = 0; i < 10; ++i) {
        vi.push_back(10);
    }
    int sum = parallel_accumulate(vi.begin(), vi.end(), 5);
    std::cout << "sum=" << sum << std::endl;
}

/**
 * 如果输入的范围为空①，就会得到init的值。反之，如果范围内多于一个元素时，都需要用范围内元素的总数量除以线程中最小任务数，
 * 从而确定启动线程的最大数量②，这样能避免无谓的计算资源的浪费。比如一台32芯的机器上，只有5个数需要计算，却启动了32个线程。
 *
 * 从计算量的最大值和硬件支持线程数中选择较小的值为启动线程的数量③。因为上下文频繁的切换会降低线程的性能，所以肯定不想启
 * 动的线程数多于硬件支持的线程数量。当std::thread::hardware_concurrency()返回0，可以选择一个合适的数作为选择；在本例中选择了"2"。
 *
 * 每个线程中处理的元素数量是范围中元素的总量除以线程的个数得出的④。通过创建一个std::vector<T>容器存放中间结果，并为线程
 * 创建std::vector<std::thread>容器⑤。
 * 需要注意，启动的线程数必须比num_threads少1个，因为在启动之前已经有了一个线程(主线程)。使用循环来启动线程：block_end迭代
 * 器指向当前块的末尾⑥，并启动新线程为当前块累加结果⑦。当迭代器指向当前块的末尾时，启动下一个块⑧。启动所有线程后，⑨中的
 * 线程会处理最终块的结果。对于分配不均，因为知道最终块是哪一个，那么这个块中有多少个元素就无所谓。当累加最终块的结果后，可
 * 以等待std::for_each ⑩创建线程的完成，之后使用std::accumulate将所有结果进行累加⑪。
 *
 * 需要明确：T类型的加法运算不满足结合律，因为对范围中元素的分组会导致parallel_accumulate得到的结果可能与std::accumulate得
 * 到的结果不同。同样，这里对迭代器的要求更加严格：必须都是向前迭代器(forward iterator)，而std::accumulate可以在只传入迭代
 * 器(input iterators)的情况下工作。对于创建出results容器，需要保证T有默认构造函数。需要注意：因为不能直接从一个线程中返回
 * 一个值，所以需要传递results容器的引用到线程中去。另一个办法通过地址来获取线程执行的结果；
 *
 * 当线程运行时，所有必要的信息都需要传入到线程中去，包括存储计算结果的位置。
 */