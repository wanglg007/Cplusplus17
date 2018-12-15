//快速排序——顺序实现版
#include <list>
#include <algorithm>

template<typename T>
std::list<T> sequential_quick_sort(std::list<T> input) {
    if (input.empty()) {
        return input;
    }
    std::list<T> result;
    result.splice(result.begin(), input, input.begin());        // 1
    T const &pivot = *result.begin();                          // 2
    auto divide_point = std::partition(input.begin(), input.end(), [&](T const &t) { return t < pivot; });  // 3

    std::list<T> lower_part;
    lower_part.splice(lower_part.end(), input, input.begin(), divide_point);         // 4
    auto new_lower(sequential_quick_sort(std::move(lower_part)));                   // 5
    auto new_higher(sequential_quick_sort(std::move(input)));                       // 6
    result.splice(result.end(), new_higher);                                         // 7
    //Using synchronization of operations to simplify code
    result.splice(result.begin(), new_lower);                                        // 8
    return result;
}

int main() {

}
/**
 * 选择第一个数为“中间”值，使用splice()①将输入的首个元素(中间值)放入结果列表中。虽然这种方式产生的结果可能不是最优的(会有大量的比较和交换操
 * 作)，但是对std::list做任何事都需要花费较长的时间，因为链表是遍历访问的。
 *
 * 现在还需要使用“中间”值进行比较，所以这里使用了一个引用②，为了避免过多的拷贝。之后，可以使用std::partition将序列中的值分成小于“中间”值的
 * 组和大于“中间”值的组③。
 *
 * 可以用splice()函数来做这件事，将input列表小于divided_point的值移动到新列表lower_part④中。其他数继续留在input列表中。而后可以使用递归调用⑤⑥的
 * 方式，对两个列表进行排序。这里显式使用std::move()将列表传递到类函数中，这种方式还是为了避免大量的拷贝操作。最终可以再次使用splice()，将result中
 * 的结果以正确的顺序进行拼接。new_higher指向的值放在“中间”值的后面⑦，new_lower指向的值放在“中间”值的前面⑧
 *
 */