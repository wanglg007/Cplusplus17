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

    barrier(unsigned count_) : count(count_), spaces(count_), generation(0) {}

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

    struct process_element {                                                            // 1
        void operator()(Iterator first, Iterator last, std::vector<value_type> &buffer, unsigned i, barrier &b) {
            value_type &ith_element = *(first + i);
            bool update_source = false;
            for (unsigned step = 0, stride = 1; stride <= i; ++step, stride *= 2) {
                value_type const &source = (step % 2) ? buffer[i] : ith_element;        // 2
                value_type &dest = (step % 2) ? ith_element : buffer[i];
                value_type const &addend = (step % 2) ? buffer[i - stride] : *(first + i - stride);     // 3
                dest = source + addend;                                                  // 4
                update_source = !(step % 2);
                b.wait();                                                                // 5
            }
            if (update_source) {                                                         // 6
                ith_element = buffer[i];
            }
            b.done_waiting();                                                            // 7
        }
    };

    unsigned long const length = std::distance(first, last);

    if (length <= 1)
        return;

    std::vector<value_type> buffer(length);
    barrier b(length);
    std::vector<std::thread> threads(length - 1);                                       // 8
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (length - 1); ++i) {
        threads[i] = std::thread(process_element(), first, last,
                                 std::ref(buffer), i, std::ref(b));                     // 9
    }
    process_element()(first, last, buffer, length - 1, b);                              // 10
}

/**
 * The overall structure of this code is probably becoming familiar by now. You have a class with a function call
 * operator (process_element) for doing the work (1), which you run on a bunch of threads (9) stored in a vector (8) and
 * which you also call from the main thread (10). The key difference this time is that the number of threads is dependent
 * on the number of items in the list rather than on std::thread::hardware_concurrency. As I said already, unless you’re
 * on a massively parallel machine where threads are cheap, this is probably a bad idea, but it shows the overall structure.
 * It would be possible to have fewer threads, with each thread handling several values from the source range, but there
 * will come a point where there are sufficiently few threads that this is less efficient than the forward-propagation
 * algorithm.
 * Anyway, the key work is done in the function call operator of process_element. At each step you either take the ith
 * element from the original range or the ith element from the buffer (2) and add it to the value stride elements prior (3),
 * storing it in the buffer if you started in the original range or back in the original range if you started in the
 * buffer (4). You then wait on the barrier (5) before starting the next step. You’ve finished when the stride takes
 * you off the start of the range, in which case you need to update the element in the original range if your final result
 * was stored in the buffer (6).Finally, you tell the barrier that you’re done_waiting() (7).
 *
 * Note that this solution isn’t exception safe. If an exception is thrown in process_element on one of the worker threads,
 * it will terminate the application. You could deal with this by using a std::promise to store the exception, as you did for
 * the parallel_find implementation from listing Ex06_09, or even just using a std::exception_ptr protected by a mutex.
 */