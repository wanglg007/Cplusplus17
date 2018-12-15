//快速排序——“期望”并行版
#include <list>
#include <algorithm>
#include <future>

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }

    std::list<T> result;
    result.splice(result.begin(), input, input.begin());
    T const &pivot = *result.begin();
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const &t) { return t < pivot ;});

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);

    std::future<std::list<T>> new_lower(std::async(&parallel_quick_sort<T>, std::move(lower_part)));        // 1
    auto new_higher(parallel_quick_sort(std::move(input)));                                                // 2

    result.splice(result.end(), new_higher);                                                                // 3
    result.splice(result.begin(), new_lower.get());                                                         // 4
    return result;
}

int main() {

}

/**
 * 当前线程不对小于“中间”值部分的列表进行排序，使用std::async()①在另一线程对其进行排序。大于部分列表使用递归的方式进行排序②。通过递归调用
 * parallel_quick_sort()就可以利用可用的硬件并发。std::async()会启动一个新线程，这样当递归三次时，就会有八个线程在运行；当你递归十次，如果硬件
 * 能处理这十次递归调用，将会创建1024个执行线程。当运行库认为这样做产生了太多的任务时，运行库可能会同步的切换新产生的任务。当任务过多时，这些
 * 任务应该在使用get()函数获取的线程上运行，而不是在新线程上运行，这样就能避免任务向线程传递的开销。这完全符合std::async的实现，为每一个任务启
 * 动一个线程(甚至在任务超额时；在std::launch::deferred没有明确规定的情况下)；或为了同步执行所有任务(在std::launch::async有明确
 * 规定的情况下)。
 *
 *
 *
 */