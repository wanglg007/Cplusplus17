//简单的栅栏类
class barrier {
    unsigned const count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;
public:
    explicit barrier(unsigned count_) :     // 1
    count(count_), spaces(count), generation(0) {}

    void wait() {
        unsigned const my_generation = generation;  // 2
        if (!--spaces) {    // 3
            spaces = count; // 4
            ++generation; //5
        } else {
            while (generation == my_generation)//6
                std::this_thread::yield();//7
        }
    }
};

/**
 * 这个实现中，用一定数量的“座位”构造了一个barrier①，这个数量将会存储count变量中。起
初，栅栏中的spaces与count数量相当。当有线程都在等待时，spaces的数量就会减少③。当
spaces的数量减到0时，spaces的值将会重置为count④，并且generation变量会增加，以向
线程发出信号，让这些等待线程能够继续运行⑤。如果spaces没有到达0，那么线程会继续等
待。这个实现使用了一个简单的自旋锁⑥，对generation的检查会在wait()开始的时候进行
②。因为generation只会在所有线程都到达栅栏的时候更新⑤，在等待的时候使用yield()⑦就
不会让CPU处于忙等待的状态。

 *
 *
 *
 *
 */