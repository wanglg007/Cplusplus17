//使用std::async实现std::for_each
template<typename Iterator, typename Func>
void parallel_for_each(Iterator first, Iterator last, Func f) {
    unsigned long const length = std::distance(first, last);

    if (!length)
        return;

    unsigned long const min_per_thread = 25;

    if (length < (2 * min_per_thread)) {
        std::for_each(first, last, f);                      // 1
    } else {
        Iterator const mid_point = first + length / 2;
        std::future<void> first_half =                      // 2
                std::async(&parallel_for_each<Iterator, Func>,
                           first, mid_point, f);
        parallel_for_each(mid_point, last, f);              // 3
        first_half.get();                                   // 4
    }
}

/**
 * As with your std::async-based parallel_accumulate from Ex06_05, you split the data recursively rather than before
 * execution, because you don’t know how many threads the library will use. As before, you divide the data in half
 * at each stage, running one half asynchronously (2) and the other directly (3) until the remaining data is too
 * small to be worth dividing, in which case you defer to std::for_each (1). Again,the use of std::async and the get()
 * member function of std::future (4) provides the exception propagation semantics.
 *
 */