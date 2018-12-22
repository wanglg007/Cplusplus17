//使用 std::async 实现的并行find算法
template<typename Iterator, typename MatchType>         // 1
Iterator parallel_find_impl(Iterator first, Iterator last, MatchType match,
                            std::atomic<bool> &done) {
    try {
        unsigned long const length = std::distance(first, last);
        unsigned long const min_per_thread = 25;        // 2
        if (length < (2 * min_per_thread)) {                // 3
            for (; (first != last) && !done.load(); ++first) {  // 4
                if (*first == match) {
                    done = true;    // 5
                    return first;   // 6
                }
            }
            return last;
        } else {
            Iterator const mid_point = first + (length / 2);    // 7
            std::future <Iterator> async_result =
                    std::async(&parallel_find_impl<Iterator, MatchType>,
                               mid_point, last, match, std::ref(done)); // 8
            Iterator const direct_result =
                    parallel_find_impl(first, mid_point, match, done);// 9
            return (direct_result == mid_point) ?
                   async_result.get() : direct_result;// 10
        }
    }
    catch (...) {
        done = true;// 11
        throw;
    }
}

template<typename Iterator, typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match) {
    std::atomic<bool> done(false);
    return parallel_find_impl(first, last, match, done);// 12
}

/**
 * 如果想要在找到匹配项时结束，就需要在线程之间设置一个标识来表明匹配项已经被找到。
因此，需要将这个标识递归的传递。通过函数①的方式来实现是最简单的办法，只需要增加一
个参数——一个done标识的引用，这个表示通过程序的主入口点传入⑫。
 *
 * 核心实现和之前的代码一样。通常函数的实现中，会让单个线程处理最少的数据项②；如果数
据块大小不足于分成两半，就要让当前线程完成所有的工作了③。实际算法在一个简单的循环
当中(给定范围)，直到在循环到指定范围中的最后一个，或找到匹配项，并对标识进行设置
④。如果找到匹配项，标识done就会在返回前进行设置⑤。无论是因为已经查找到最后一
个，还是因为其他线程对done进行了设置，都会停止查找。如果没有找到，会将最后一个元
素last进行返回⑥。
 *
 * 如果给定范围可以进行划分，首先要在 st::async 在对第二部分进行查找⑧前，要找数据中点
⑦，而且需要使用 std::ref 将done以引用的方式传递。同时，可以通过对第一部分直接进行
递归查找。两部分都是异步的，并且在原始范围过大时，直接递归查找的部分可能会再细
化。
 *
 * 如果直接查找返回的是mid_point，这就意味着没有找到匹配项，所以就要从异步查找中获取
结果。如果在另一半中没有匹配项的话，返回的结果就一定是last，这个值的返回就代表了没
有找到匹配的元素⑩。如果“异步”调用被延迟(非真正的异步)，那么实际上这里会运行get()；
 这种情况下，如果对下半部分的元素搜索成功，那么就不会执行对上半部分元素的搜索了。
如果异步查找真实的运行在其他线程上，那么async_result变量的析构函数将会等待该线程完
成，所以这里不会有线程泄露。
 *
 * 像之前一样， std::async 可以用来提供“异常-安全”和“异常-传播”特性。如果直接递归抛出异
常，future的析构函数就能让异步执行的线程提前结束；如果异步调用抛出异常，那么这个异
常将会通过对get()成员函数的调用进行传播⑩。使用try/catch块只能捕捉在done发生的异
常，并且当有异常抛出⑪时，所有线程都能很快的终止运行。不过，不使用try/catch的实现依
旧没问题，不同的就是要等待所有线程的工作是否完成。

 *
 * 实现中一个重要的特性就是，不能保证所有数据都能被 std::find 串行处理。其他并行算法
可以借鉴这个特性，因为要让一个算法并行起来这是必须具有的特性。如果有顺序问题，元
素就不能并发的处理了。如果每个元素独立，虽然对于parallel_for_each不是很重要，不过对
于parallel_find，即使在开始部分已经找到了匹配元素，也有可能返回范围中最后一个元素；
如果在知道结果的前提下，这样的结果会让人很惊讶。
 */
