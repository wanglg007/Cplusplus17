//使用划分的方式来并行的计算部分和
template<typename Iterator>
void parallel_partial_sum(Iterator first, Iterator last) {
    typedef typename Iterator::value_type value_type;
    struct process_chunk {// 1
        void operator()(Iterator begin, Iterator last,
                        std::future <value_type> *previous_end_value,
                        std::promise <value_type> *end_value) {
            try {
                Iterator end = last;
                ++end;
                std::partial_sum(begin, end, begin);// 2
                if (previous_end_value) {// 3
                    value_type &addend = previous_end_value->get();// 4
                    *last += addend;// 5
                    if (end_value) {
                        end_value->set_value(*last);// 6
                    }
                    std::for_each(begin, last, [addend](value_type &item) {
                        item += addend;
                    });// 7
                } else if (end_value) {
                    end_value->set_value(*last);// 8
                }
            }
            catch (...) {// 9
                if (end_value) {
                    end_value->set_exception(std::current_exception());// 10
                } else {
                    throw;// 11
                }
            }
        }
    };

    unsigned long const length = std::distance(first, last);

    if (!length)
        return last;

    unsigned long const min_per_thread = 25;// 12
    unsigned long const max_threads =
            (length + min_per_thread - 1) / min_per_thread;

    unsigned long const hardware_threads =
            std::thread::hardware_concurrency();

    unsigned long const num_threads =
            std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);

    unsigned long const block_size = length / num_threads;

    typedef typename Iterator::value_type value_type;

    std::vector <std::thread> threads(num_threads - 1);// 13
    std::vector <std::promise<value_type>>
            end_values(num_threads - 1);// 14
    std::vector <std::future<value_type>>
            previous_end_values;// 15
    previous_end_values.reserve(num_threads - 1);// 16
    join_threads joiner(threads);

    Iterator block_start = first;
    for (unsigned long i = 0; i < (num_threads - 1); ++i) {
        Iterator block_last = block_start;
        std::advance(block_last, block_size - 1);// 17
        threads[i] = std::thread(process_chunk(),// 18
                                 block_start, block_last,
                                 (i != 0) ? &previous_end_values[i - 1] : 0,
                                 &end_values[i]);
        block_start = block_last;
        ++block_start;// 19
        previous_end_values.push_back(end_values[i].get_future());
    }
    Iterator final_element = block_start;
    std::advance(final_element, std::distance(block_start, last) - 1);
    process_chunk()(block_start, final_element,// 22
                    (num_threads > 1) ? &previous_end_values.back() : 0,
                    0);
}

/**
 *这个实现中，使用的结构体和之前算法中的一样，将问题进行分块解决，每个线程处理最小
的数据块⑫。其中，有一组线程⑬和一组promise⑭，用来存储每块中的最后一个值；并且实
现中还有一组future⑮，用来对前一块中的最后一个值进行检索。可以为future⑯做些储备，
以避免生成新线程时，再分配内存。
 *
 * 主循环和之前一样，不过这次是让迭代器指向了每个数据块的最后一个元素，而不是作为一
个普通值传递到最后⑰，这样就方便向其他块传递当前块的最后一个元素了。实际处理是在
process_chunk函数对象中完成的，这个结构体看上去不是很长；当前块的开始和结束迭代器
和前块中最后一个值的future一起，作为参数进行传递，并且promise用来保留当前范围内最
后一个值的原始值⑱。
 *
 * 生成新的线程后，就对开始块的ID进行更新，别忘了传递最后一个元素⑲，并且将当前块的
最后一个元素存储到future，上面的数据将在循环中再次使用到⑳。
在处理最后一个数据块前，需要获取之前数据块中最后一个元素的迭代器(21)，这样就可以将
其作为参数传入process_chunk(22)中了。 std::partial_sum 不会返回一个值，所以在最后一
个数据块被处理后，就不用再做任何事情了。当所有线程的操作完成时，求部分和的操作也
就算完成了
 *
 * OK，现在来看一下process_chunk函数对象①。对于整块的处理是始于
对 std::partial_sum 的调用，包括对于最后一个值的处理②，不过得要知道当前块是否是第
一块③。如果当前块不是第一块，就会有一个previous_end_value值从前面的块传过来，所以
这里需要等待这个值的产生④。为了将算法最大程度的并行，首先需要对最后一个元素进行更
新⑤，这样你就能将这个值传递给下一个数据块(如果有下一个数据块的话)⑥。当完成这个操
作，就可以使用 std::for_each 和简单的lambda函数⑦对剩余的数据项进行更新。
 *
 * 如果previous_end_value值为空，当前数据块就是第一个数据块，所以只需要为下一个数据
块更新end_value⑧(如果有下一个数据块的话——当前数据块可能是唯一的数据块)。
最后，如果有任意一个操作抛出异常，就可以将其捕获⑨，并且存入promise⑩，如果下一个
数据块尝试获取前一个数据块的最后一个值④时，异常会再次抛出。处理最后一个数据块时，
异常会全部重新抛出⑪，因为抛出动作一定会在主线程上进行。
 */