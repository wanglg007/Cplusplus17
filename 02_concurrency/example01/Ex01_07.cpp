//(1.7)使用层次锁来避免死锁
#include <mutex>

class hierarchical_mutex {
public:
    explicit hierarchical_mutex(unsigned level) {
    }

    void lock() {
    }

    void unlock() {}
};

hierarchical_mutex high_level_mutex(10000);     // 1
hierarchical_mutex low_level_mutex(5000);       // 2

int do_low_level_stuff() {
    return 42;
}

int low_level_func() {
    std::lock_guard<hierarchical_mutex> lk(low_level_mutex);    // 3
    return do_low_level_stuff();
}

int high_level_stuff(int some_param) {

}

void high_level_func() {
    std::lock_guard<hierarchical_mutex> lk(high_level_mutex);   // 4
    high_level_stuff(low_level_func());                         // 5
}

void thread_a() {                                              // 6
    high_level_func();
}

hierarchical_mutex other_mutex(100);                            // 7

void do_other_stuff() {

}

void other_stuff() {
    high_level_func();                                          // 8
    do_other_stuff();
}

void thread_b() {                                              // 9
    std::lock_guard<hierarchical_mutex> lk(other_mutex);        // 10
    other_stuff();
}

int main() {

}

/**
 * thread_a()⑥遵守规则，所以运行没问题。而thread_b()⑨无视规则，因此运行时肯定会失败。
 *
 * thread_a()调用high_level_func()，让high_level_mutex④上锁(其层级值为10000①)，为了获取high_level_stuff()的参数对互斥量上锁，之后调用low_level_func()⑤。
 * low_level_func()会对low_level_mutex上锁，这就没有问题，因为这个互斥量有一个低层值5000②。
 *
 * thread_b()运行不会顺利。首先，它锁住了other_mutex⑩，这个互斥量的层级值只有100⑦。这意味着超低层级的数据(ultra-low-level data)已被保护。当other_stuff()调用
 * high_level_func()⑧时，就违反了层级结构：high_level_func()试图获取high_level_mutex，这个互斥量的层级值是10000，要比当前层级值100大很多。因此hierarchical_mutex
 * 将会产生一个错误，可能会是抛出一个异常，或直接终止程序。在层级互斥量上产生死锁，是不可能的，因为互斥量本身会严格遵循约定顺序，进行上锁。这也意味，当多个互斥量
 * 在是在同一级上时，不能同时持有多个锁，所以“手递手”锁的方案需要每个互斥量在一条链上，并且每个互斥量都比其前一个有更低的层级值，这在某些情况下无法实现。
 *
 *
 */