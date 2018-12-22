//使用 std::async 实现 std::for_each
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return;

    unsigned long const min_per_thread = 25;

    if (length < (2 * min_per_thread)) {
        std::for_each(first, last, f);  // 1
    } else {
        Iterator const mid_point = first + length / 2;
        std::future<void> first_half =      // 2
                std::async(&parallel_for_each<Iterator, Func>,
                           first, mid_point, f);
        parallel_for_each(mid_point, last, f);  // 3
        first_half.get();   // 4
    }
}

/**
 * 和基于 std::async 的parallel_accumulate(清单8.5)一样，是在运行时对数据进行迭代划分
的，而非在执行前划分好，这是因为你不知道你的库需要使用多少个线程。像之前一样，当
你将每一级的数据分成两部分，异步执行另外一部分②，剩下的部分就不能再进行划分了，所
以直接运行这一部分③；这样就可以直接对 std::for_each ①进行使用了。这里再次使
用 std::async 和 std::future 的get()成员函数④来提供对异常的传播。
 *
 *
 */