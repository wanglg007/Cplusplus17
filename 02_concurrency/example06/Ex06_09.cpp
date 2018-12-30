//并行std::find算法实现
template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match) {
    struct find_element {                                                   // 1
        void operator()(Iterator begin, Iterator end, MatchType match, std::promise <Iterator> *result,
                        std::atomic<bool> *done_flag) {
            try {
                for (; (begin != end) && !done_flag->load(); ++begin) {      // 2
                    if (*begin == match) {
                        result->set_value(begin);                            // 3
                        done_flag->store(true);                             // 4
                        return;
                    }
                }
            }
            catch (...) {                                                   // 5
                try {
                    result->set_exception(std::current_exception());         // 6
                    done_flag->store(true);
                }
                catch (...) {}                                              // 7
            }
        }
    };

    unsigned long const length = std::distance(first, last);

    if (!length)
        return last;

    unsigned long const min_per_thread = 25;
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;
    unsigned long const hardware_threads = std::thread::hardware_concurrency();
    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
    unsigned long const block_size = length / num_threads;

    std::promise <Iterator> result;                                           // 8
    std::atomic<bool> done_flag(false);                                      // 9
    std::vector <std::thread> threads(num_threads - 1);
    {                                                                         // 10
        join_threads joiner(threads);

        Iterator block_start = first;
        for (unsigned long i = 0; i < (num_threads - 1); ++i) {
            Iterator block_end = block_start;
            std::advance(block_end, block_size);
            threads[i] = std::thread(find_element(), block_start, block_end, match, &result, &done_flag);   // 11
            block_start = block_end;
        }
        find_element()(block_start, last, match, &result, &done_flag);       // 12
    }
    if (!done_flag.load()) {                                                 // 13
        return last;
    }
    return result.get_future().get();                                       // 14
}

/**
 * The main body of listing Ex06_09 is similar to the previous examples. This time, the work is done in the function call
 * operator of the local find_element class (1). This loops through the elements in the block it’s been given, checking
 * the flag at each step (2). If a match is found, it sets the final result value in the promise (3) and then sets the
 * done_flag (4) before returning.
 *
 * If an exception is thrown, this is caught by the catchall handler (5), and you try to store the exception in the promise (6)
 * before setting the done_flag. Setting the value on the promise might throw an exception if the promise is already set,
 * so you catch and discard any exceptions that happen here (7).
 *
 * This means that if a thread calling find_element either finds a match or throws an exception, all other threads will
 * see done_flag set and will stop. If multiple threads find a match or throw at the same time, they’ll race to set the
 * result in the promise.But this is a benign race condition; whichever succeeds is therefore nominally “first” and is
 * therefore an acceptable result.
 *
 * Back in the main parallel_find function itself, you have the promise (8) and flag (9) used to stop the search, both of
 * which are passed in to the new threads along with the range to search (11). The main thread also uses find_element to
 * search the remaining elements (12). As already mentioned, you need to wait for all threads to finish before you check
 * the result, because there might not be any matching elements. You do this by enclosing the thread launching-and-joining
 * code in a block (10), so all threads are joined when you check the flag to see whether a match was found (13). If a match
 * was found, you can get the result or throw the stored exception by calling get() on the std::future<Iterator> you can
 * get from the promise (14)
 *
 * Again, this implementation assumes that you’re going to be using all available hardware threads or that you have some
 * other mechanism to determine the number of threads to use for the up-front division of work between threads. Just as
 * before, you can use std::async and recursive data division to simplify your implementation, while using the automatic
 * scaling facility of the C++ Standard Library. An implementation of parallel_find using std::async is shown in the
 * following listing.
 */