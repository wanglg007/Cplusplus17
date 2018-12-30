//使用std::async实现的并行find算法
template<typename Iterator, typename MatchType>
// 1
Iterator parallel_find_impl(Iterator first, Iterator last, MatchType match,
                            std::atomic<bool> &done) {
    try {
        unsigned long const length = std::distance(first, last);
        unsigned long const min_per_thread = 25;                // 2
        if (length < (2 * min_per_thread)) {                      // 3
            for (; (first != last) && !done.load(); ++first) {    // 4
                if (*first == match) {
                    done = true;                                  // 5
                    return first;                                 // 6
                }
            }
            return last;
        } else {
            Iterator const mid_point = first + (length / 2);      // 7
            std::future <Iterator> async_result = std::async(&parallel_find_impl<Iterator, MatchType>,
                                                             mid_point, last, match, std::ref(done));   // 8
            Iterator const direct_result = parallel_find_impl(first, mid_point, match, done);          // 9
            return (direct_result == mid_point) ? async_result.get() : direct_result;                  // 10
        }
    }
    catch (...) {
        done = true;                                                                                   // 11
        throw;
    }
}

template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match) {
    std::atomic<bool> done(false);
    return parallel_find_impl(first, last, match, done);                                               // 12
}

/**
 * The desire to finish early if you find a match means that you need to introduce a flag that is shared between all threads
 * to indicate that a match has been found. This therefore needs to be passed in to all recursive calls. The simplest way
 * to achieve this is by delegating to an implementation function (1) that takes an additional parameter—a reference to the
 * done flag, which is passed in from the main entry point (12).
 *
 * The core implementation then proceeds along familiar lines. In common with many of the implementations here, you set a
 * minimum number of items to process on a single thread (2); if you can’t cleanly divide into two halves of at least that
 * size, you run everything on the current thread (3). The actual algorithm is a simple loop through the specified range,
 * looping until you reach the end of the range or the done flag is set (4). If you do find a match, the done flag is set
 * before returning (5). If you stop searching either because you got to the end of the list or because another thread
 * set the done flag, you return last to indicate that no match was found here (6).
 *
 * If the range can be divided, you first find the midpoint h before using std::async to run the search in the second half
 * of the range (7), being careful to use std::ref to pass a reference to the done flag. In the meantime, you can search
 * in the first half of the range by doing a direct recursive call (8). Both the asynchronous call and the direct recursion
 * may result in further subdivisions if the original range is big enough.
 *
 * If the direct search returned mid_point, then it failed to find a match, so you need to get the result of the asynchronous
 * search. If no result was found in that half,the result will be last, which is the correct return value to indicate that
 * the value was not found (10). If the “asynchronous” call was deferred rather than truly asynchronous, it will actually
 * run here in the call to get(); in such circumstances the search of the top half of the range is skipped if the search
 * in the bottom half was successful. If the asynchronous search is really running on another thread, the destructor of
 * the async_result variable will wait for the thread to complete, so you don’t have any leaking threads.
 *
 * As before, the use of std::async provides you with exception-safety and exceptionpropagation features. If the direct
 * recursion throws an exception, the future’s destructor will ensure that the thread running the asynchronous call has
 * terminated before the function returns, and if the asynchronous call throws, the exception is propagated through the
 * get() call (10). The use of a try/catch block around the whole thing is only there to set the done flag on an exception
 * and ensure that all threads terminate quickly if an exception is thrown (11). The implementation would still be correct
 * without it but would keep checking elements until every thread was finished.
 */
