//使用栈的并行快速排序算法——等待数据块排序
#include <list>
#include <thread>
#include <future>
#include <vector>
#include <memory>
#include <algorithm>

template<typename T>
struct sorter                                       // 1
{
    struct chunk_to_sort {
        std::list<T> data;
        std::promise<std::list<T>> promise;
    };

    thread_safe_stack<chunk_to_sort> chunks;        // 2
    std::vector<std::thread> threads;               // 3
    unsigned const max_thread_count;
    std::atomic<bool> end_of_data;

    sorter() : max_thread_count(std::thread::hardware_concurrency() - 1), end_of_data(false) {}

    ~sorter() {                                     // 4
        end_of_data = true;                        // 5
        for (unsigned i = 0; i < threads.size(); ++i) {
            threads[i].join();                      // 6
        }
    }

    void try_sort_chunk() {
        std::shared_ptr<chunk_to_sort> chunk = chunks.pop();   // 7
        if (chunk) {
            sort_chunk(chunk);                                  // 8
        }
    }

    std::list<T> do_sort(std::list<T> &chunk_data)              // 9
    {
        if (chunk_data.empty()) {
            return chunk_data;
        }

        std::list<T> result;
        result.splice(result.begin(), chunk_data, chunk_data.begin());
        T const &partition_val = *result.begin();

        typename std::list<T>::iterator divide_point = std::partition(chunk_data.begin(), chunk_data.end(),
                                                                      [&](T const &val) {
                                                                          return val < partition_val;
                                                                      });    // 10
        chunk_to_sort new_lower_chunk;
        new_lower_chunk.data.splice(new_lower_chunk.data.end(), chunk_data, chunk_data.begin(), divide_point);

        std::future<std::list<T>> new_lower = new_lower_chunk.promise.get_future();
        chunks.push(std::move(new_lower_chunk));                            // 11
        if (threads.size() < max_thread_count) {                            // 12
            threads.push_back(std::thread(&sorter<T>::sort_thread, this));
        }

        std::list<T> new_higher(do_sort(chunk_data));

        result.splice(result.end(), new_higher);
        while (new_lower.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {      // 13
            try_sort_chunk();                                                // 14
        }

        result.splice(result.begin(), new_lower.get());
        return result;
    }

    void sort_chunk(boost::shared_ptr <chunk_to_sort> const &chunk) {
        chunk->promise.set_value(do_sort(chunk->data)); // 15
    }

    void sort_thread() {
        while (!end_of_data) {                         // 16
            try_sort_chunk();                           // 17
            std::this_thread::yield();                  // 18
        }
    }
};

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {  // 19
    if (input.empty()) {
        return input;
    }
    sorter<T> s;
    return s.do_sort(input);                           // 20
}

/**
 * Tthe parallel_quick_sort function(19)(delegates most of the functionality to the sorter class (1), which provides an easy way of
 * grouping the stack of unsorted chunks (2) and the set of threads (3). The main work is done in the do_sort memberfunction (9),
 * which does the usual partitioning of the data (10). This time, rather than spawning a new thread for one chunk, it pushes it onto
 * the stack (11) and spawns a new thread while you still have processors to spare (12). Because the lower chunk might be handled by
 * another thread, you then have to wait for it to be ready (13). In order to help things along (in case you’re the only thread or
 * all the others are already busy), you try to process chunks from the stack on this thread while you’re waiting (14).try_sort_chunk
 * just pops a chunk off the stack (7) and sorts it (8), storing the result in the promise, ready to be picked up by the thread that
 * posted the chunk on the stack (15).
 *
 * Your freshly spawned threads sit in a loop trying to sort chunks off the stack (17) while the end_of_data flag isn’t set (16).
 * In between checking, they yield to other threads (18) to give them a chance to put some more work on the stack. This code relies
 * on the destructor of your sorter class (4) to tidy up these threads. When all the data has been sorted, do_sort will return (even
 * though the worker threads are still running), so your main thread will return from parallel_quick_sort (20) and thus destroy your
 * sorter object. This sets the end_of_data flag (5) and waits for the threads to finish (6). Setting the flag terminates the loop
 * in the thread function (16).
 *
 */