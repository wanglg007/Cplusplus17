//异常安全并行版 std::accumulate ——使用 std::async()
template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
    unsigned long const length = std::distance(first, last);       // 1
    unsigned long const max_chunk_size = 25;
    if (length <= max_chunk_size) {
        return std::accumulate(first, last, init);                  // 2
    } else {
        Iterator mid_point = first;
        std::advance(mid_point, length / 2);                        // 3
        std::future <T> first_half_result =
                std::async(parallel_accumulate<Iterator, T>, first, mid_point, init);   // 4
        T second_half_result = parallel_accumulate(mid_point, last, T());
        return first_half_result.get() + second_half_result;        // 6
    }
}

/**
 * 这个版本对数据进行递归划分，而非在预计算后对数据进行分块；因此，这个版本要比之前
的版本简单很多，并且这个版本也是异常安全的。和之前一样，一开始要确定序列的长度①，
如果其长度小于数据块包含数据的最大数量，那么可以直接调用 std::accumulate ②。如果元
素的数量超出了数据块包含数据的最大数量，那么就需要找到数量中点③，将这个数据块分成
两部分，然后再生成一个异步任务对另一半数据进行处理④。第二半的数据是通过直接的递归
调用来处理的⑤，之后将两个块的结果加和到一起⑥。标准库能保证 std::async 的调用能够
充分的利用硬件线程，并且不会产生线程的超额认购，一些“异步”调用是在调用get()⑥后同步
执行的。
 *
 *优雅的地方，不仅在于利用硬件并发的优势，并且还能保证异常安全。如果有异常在递归调
用⑤中抛出，通过调用 std::async ④所产生的“期望”，将会在异常传播时被销毁。这就需要依
次等待异步任务的完成，因此也能避免悬空线程的出现。另外，当异步任务抛出异常，且被
future所捕获，在对get()⑥调用的时候，future中存储的异常，会再次抛出。
 *
 *
 *
 */