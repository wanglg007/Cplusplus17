//(1.10)在比较操作符中一次锁住一个互斥量
#include <mutex>

class Y {
private:
    int some_detail;
    mutable std::mutex m;

    int get_detail() const {
        std::lock_guard<std::mutex> lock_a(m);          // 1
        return some_detail;
    }

public:
    Y(int sd) : some_detail(sd) {}

    friend bool operator==(Y const &lhs, Y const &rhs) {
        if (&lhs == &rhs)
            return true;
        int const lhs_value = lhs.get_detail();         // 2
        int const rhs_value = rhs.get_detail();         // 3
        return lhs_value == rhs_value;                  // 4
    }
};

int main() {}

/**
 * 在该示例中，比较操作符首先通过调用get_detail()成员函数检索要比较的值②③。函数在索引值时被一个锁保护着①。比较操作符会在之后比较索引出来的值④。
 * 注意：虽然这样能减少锁持有的时间，一个锁只持有一次(这样能消除死锁的可能性)，这里有一个微妙的语义操作同时对两个锁住的值进行比较。当操作符返回true时，
 * 那就意味着在这个时间点上的lhs.some_detail与在另一个时间点的rhs.some_detail相同。这两个值在读取之后，可能会被任意的方式所修改；两个值会在②和③出进行
 * 交换，这样的话会失去比较的意义。等价比较可能会返回true，来表明这两个值时相等的，实际上这两个值相等的情况可能就发生在一瞬间。这样的变化要小心，语义
 * 操作是无法改变一个问题的比较方式的：当你持有锁的时间没有达到整个操作时间时，你就会让自己处于条件竞争的状态。
 *
 * 有时只是没有一个合适粒度级别，因为并不是所有对数据结构的访问都需要同一级的保护。在这个例子中，就需要寻找一个合适的机制去替换std::mutex 。
 */
