//并行find算法实现
template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match) {
    struct find_element {           // 1
        void operator()(Iterator begin, Iterator end,
                        MatchType match,
                        std::promise <Iterator> *result,
                        std::atomic<bool> *done_flag) {
            try {
                for (; (begin != end) && !done_flag->load(); ++begin) { // 2
                    if (*begin == match) {
                        result->set_value(begin);       // 3
                        done_flag->store(true);         // 4
                        return;
                    }
                }
            }
            catch (...) {   // 5
                try {
                    result->set_exception(std::current_exception());        // 6
                    done_flag->store(true);
                }
                catch (...) {}  // 7
            }
        }
    };

    unsigned long const length = std::distance(first, last);

    if (!length)
        return last;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads =
            (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads =
            std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    std::promise <Iterator> result;     // 8
    std::atomic<bool> done_flag(false); // 9
    std::vector <std::thread> threads(num_threads - 1);
    {   // 10
        join_threads joiner(threads);

        Iterator block_start = first;
        for (unsigned long i = 0; i < (num_threads - 1); ++i) {
            Iterator block_end = block_start;
            std::advance(block_end, block_size);
            threads[i] = std::thread(find_element(),
                                     block_start, block_end, match,
                                     &result, &done_flag);// 11
            block_start = block_end;
        }
        find_element()(block_start, last, match, &result, &done_flag);//12
    }
    if (!done_flag.load()) {//13
        return last;
    }
    return result.get_future().get();// 14
}

/**
 *由find_element类①的函数调用操作实现，
来完成查找工作的。循环通过在给定数据块中的元素，检查每一步上的标识②。如果匹配的元
素被找到，就将最终的结果设置到promise③当中，并且在返回前对done_flag④进行设置。
 *
 * 如果有一个异常被抛出，那么它就会被通用处理代码⑤捕获，并且在promise⑥尝中试存储
前，对done_flag进行设置。如果对应promise已经被设置，设置在promise上的值可能会抛出
一个异常，所以这里⑦发生的任何异常，都可以捕获并丢弃。
 *
 * 这意味着，当线程调用find_element查询一个值，或者抛出一个异常时，如果其他线程看到
done_flag被设置，那么其他线程将会终止。如果多线程同时找到匹配值或抛出异常，它们将
会对promise产生竞争。不过，这是良性的条件竞争；因为，成功的竞争者会作为“第一个”返
回线程，因此这个结果可以接受。
 *
 *回到parallel_find函数本身，其拥有用来停止搜索的promise⑧和标识⑨；随着对范围内的元素
的查找⑪，promise和标识会传递到新线程中。主线程也使用find_element来对剩下的元素进
行查找⑫。像之前提到的，需要在全部线程结束前，对结果进行检查，因为结果可能是任意
位置上的匹配元素。这里将“启动-汇入”代码放在一个块中⑩，所以所有线程都会在找到匹配元
素时⑬进行汇入。如果找到匹配元素，就可以调用 std::future<Iterator> (来自promise⑭)的
成员函数get()来获取返回值或异常。
 */