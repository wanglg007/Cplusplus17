//通过两两更新对的方式实现partial_sum
#include <atomic>
#include <thread>
#include <vector>

struct join_threads {
    join_threads(std::vector<std::thread> &) {}
};


struct barrier {
    std::atomic<unsigned> count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;

    barrier(unsigned count_) :
            count(count_), spaces(count_), generation(0) {}

    void wait() {
        unsigned const gen = generation.load();
        if (!--spaces) {
            spaces = count.load();
            ++generation;
        } else {
            while (generation.load() == gen) {
                std::this_thread::yield();
            }
        }
    }

    void done_waiting() {
        --count;
        if (!--spaces) {
            spaces = count.load();
            ++generation;
        }
    }
};

template<typename Iterator>
void parallel_partial_sum(Iterator first, Iterator last) {
    typedef typename Iterator::value_type value_type;

    struct process_element {
        void operator()(Iterator first, Iterator last,
                        std::vector<value_type> &buffer,
                        unsigned i, barrier &b) {
            value_type &ith_element = *(first + i);
            bool update_source = false;
            for (unsigned step = 0, stride = 1; stride <= i; ++step, stride *= 2) {
                value_type const &source = (step % 2) ?
                                           buffer[i] : ith_element;
                value_type &dest = (step % 2) ?
                                   ith_element : buffer[i];
                value_type const &addend = (step % 2) ?
                                           buffer[i - stride] : *(first + i - stride);
                dest = source + addend;
                update_source = !(step % 2);
                b.wait();
            }
            if (update_source) {
                ith_element = buffer[i];
            }
            b.done_waiting();
        }
    };

    unsigned long const length = std::distance(first, last);

    if (length <= 1)
        return;

    std::vector<value_type> buffer(length);
    barrier b(length);
    std::vector<std::thread> threads(length - 1);
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (length - 1); ++i) {
        threads[i] = std::thread(process_element(), first, last,
                                 std::ref(buffer), i, std::ref(b));
    }
    process_element()(first, last, buffer, length - 1, b);
}

/**
 * 代码的整体结构应该不用说了。process_element类有函数调用操作可以用来做具体的工作
①，就是运行一组线程⑨，并将线程存储到vector中⑧，同样还需要在主线程中对其进行调用
⑩。这里与之前最大的区别就是，线程的数量是根据列表中的数据量来定的，而非根
据 std::thread::hardware_concurrency 。如我之前所说，除非你使用的是一个大规模并行的机
器，因为这上面的线程都十分廉价(虽然这样的方式并不是很好)，还能为我们展示了其整体结
构。这个结构在有较少线程的时候，每一个线程只能处理源数据中的部分数据，当没有足够
的线程支持该结构时，效率要比传递算法低。
 *
 * 不管怎样，主要的工作都是调用process_element的函数操作符来完成的。每一步，都会从原
始数据或缓存中获取第i个元素②，并且将获取到的元素加到指定stride的元素中去③，如果从
原始数据开始读取的元素，加和后的数需要存储在缓存中④。然后，在开始下一步前，会在栅
栏处等待⑤。当stride超出了给定数据的范围，当最终结果已经存在缓存中时，就需要更新原
始数据中的数据，同样这也意味着本次加和结束。最后，在调用栅栏中的done_waiting()函数
⑦。
 *
 *注意这个解决方案并不是异常安全的。如果某个线程在process_element执行时抛出一个异
常，其就会终止整个应用。这里可以使用一个 std::promise 来存储异常，就像在清单8.9中
parallel_find的实现，或仅使用一个被互斥量保护的 std::exception_ptr 即可。
 *
 */