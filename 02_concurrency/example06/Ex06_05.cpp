//异常安全并行版std::accumulate ——使用 std::async()
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);                         // 1  确定序列的长度
    unsigned long const max_chunk_size = 25;
    if (length <= max_chunk_size) {
        return std::accumulate(first, last, init);                                     // 2
    } else {
        Iterator mid_point = first;
        std::advance(mid_point, length / 2);                                            // 3 找到数量中点
        std::future <T> first_half_result =
                std::async(parallel_accumulate<Iterator, T>, first, mid_point, init);   // 4
        T second_half_result = parallel_accumulate(mid_point, last, T());               // 5
        return first_half_result.get() + second_half_result;                           // 6
    }
}

/**
 * This version uses recursive division of the data rather than pre-calculating the division of the data into chunks, but
 * it’s a whole lot simpler than the previous version, and it’s still exception safe. As before, you start by finding
 * the length of the sequence (1), and if it’s smaller than the maximum chunk size, you resort to calling std::accumulate
 * directly (2). If there are more elements than your chunk size, you find the midpoint (3) and then spawn an asynchronous
 * task to handle that half (4). The second half of the range is handled with a direct recursive call (5), and then the
 * results from the two chunks are added together (6). The library ensures that the std::async calls make use of the hardware
 * threads that are available without creating an overwhelming number of threads. Some of the “asynchronous” calls will
 * actually be executed synchronously in the call to get() (6).
 *
 * The beauty of this is that not only can it take advantage of the hardware concurrency, but it’s also trivially exception
 * safe. If an exception is thrown by the recursive call (5), the future created from the call to std::async (4) will be
 * destroyed as the exception propagates. This will in turn wait for the asynchronous task to finish, thus avoiding a dangling
 * thread. On the other hand, if the asynchronous call throws, this is captured by the future, and the call to get() (6) will
 * rethrow the exception.
 *
 */