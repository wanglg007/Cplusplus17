//使用划分的方式来并行的计算部分和
template<typename Iterator>
void parallel_partial_sum(Iterator first, Iterator last) {
    typedef typename Iterator::value_type value_type;
    struct process_chunk {                                              // 1
        void operator()(Iterator begin, Iterator last, std::future <value_type> *previous_end_value,
                        std::promise <value_type> *end_value) {
            try {
                Iterator end = last;
                ++end;
                std::partial_sum(begin, end, begin);                    // 2
                if (previous_end_value) {                               // 3
                    value_type &addend = previous_end_value->get();     // 4
                    *last += addend;                                    // 5
                    if (end_value) {
                        end_value->set_value(*last);                    // 6
                    }
                    std::for_each(begin, last, [addend](value_type &item) {
                        item += addend;
                    });                                                 // 7
                } else if (end_value) {
                    end_value->set_value(*last);                        // 8
                }
            }
            catch (...) {                                               // 9
                if (end_value) {
                    end_value->set_exception(std::current_exception()); // 10
                } else {
                    throw;                                              // 11
                }
            }
        }
    };

    unsigned long const length = std::distance(first, last);

    if (!length)
        return last;

    unsigned long const min_per_thread = 25;                    // 12
    unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads = std::thread::hardware_concurrency();

    unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    typedef typename Iterator::value_type value_type;

    std::vector <std::thread> threads(num_threads - 1);                 // 13
    std::vector <std::promise<value_type>> end_values(num_threads - 1); // 14
    std::vector <std::future<value_type>> previous_end_values;          // 15
    previous_end_values.reserve(num_threads - 1);                       // 16
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_last = block_start;
        std::advance(block_last, block_size - 1);                       // 17
        threads[i] = std::thread(process_chunk(),                       // 18
                                 block_start, block_last,
                                 (i != 0) ? &previous_end_values[i - 1] : 0,
                                 &end_values[i]);
        block_start = block_last;
        ++block_start;                                                  // 19
        previous_end_values.push_back(end_values[i].get_future());      // 20
    }
    Iterator final_element = block_start;
    std::advance(final_element, std::distance(block_start, last) - 1);  // 21
    process_chunk()(block_start, final_element, (num_threads > 1) ? &previous_end_values.back() : 0, 0);    // 22
}

/**
 * In this instance, the general structure is the same as with the previous algorithms,dividing the problem into chunks,
 * with a minimum chunk size per thread (12). In this case, as well as the vector of threads (13), you have a vector of
 * promises (14), which is used to store the value of the last element in the chunk, and a vector of futures (15), which
 * is used to retrieve the last value from the previous chunk. You can reserve the space for the futures (16) to avoid a
 * reallocation while spawning threads, because you know how many you’re going to have.
 *
 * The main loop is the same as before, except this time you actually want the iterator that points to the last element
 * in each block, rather than being the usual one past the end (17), so that you can do the forward propagation of the
 * last element in each range.The actual processing is done in the process_chunk function object, which we’ll look at
 * shortly; the start and end iterators for this chunk are passed in as arguments alongside the future for the end value
 * of the previous range (if any) and the promise to hold the end value of this range (18).
 *
 * After you’ve spawned the thread, you can update the block start, remembering to advance it past that last element (19),
 * and store the future for the last value in the current chunk into the vector of futures so it will be picked up next
 * time around the loop (20). Before you process the final chunk, you need to get an iterator for the last element (21),
 * which you can pass in to process_chunk (22). std::partial_sum doesn’t return a value, so you don’t need to do anything
 * once the final chunk has been processed. The operation is complete once all the threads have finished.
 *
 * OK, now it’s time to look at the process_chunk function object that actually does all the work (1). You start by calling
 * std::partial_sum for the entire chunk, including the final element (2), but then you need to know if you’re the first
 * chunk or not (3). If you are not the first chunk, then there was a previous_end_value from the previous chunk, so you
 * need to wait for that (4). In order to maximize the parallelism of the algorithm, you then update the last element
 * first (5), so you can pass the value on to the next chunk (if there is one) (6). Once you’ve done that, you can just
 * use std::for_each and a simple lambda function (7) to update all the remaining elements in the range.
 *
 * If there was not a previous_end_value, you’re the first chunk, so you can just update the end_value for the next
 * chunk (again, if there is one—you might be the only chunk) (8).
 *
 * Finally, if any of the operations threw an exception, you catch it (9) and store it in the promise (10) so it will
 * propagate to the next chunk when it tries to get the previous end value (4). This will propagate all exceptions into
 * the final chunk, which then just rethrows (11), because you know you’re running on the main thread.
 *
 * Because of the synchronization between the threads, this code isn’t readily amenable to rewriting with std::async.
 * The tasks wait on results made available partway through the execution of other tasks, so all tasks must be running
 * concurrently. With the block-based, forward-propagation approach out of the way, let’s look at the second approach to
 * computing the partial sums of a range.
 *
 */