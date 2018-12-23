//get_hazard_pointer_for_current_thread()函数的简单实现
#include <atomic>
#include <thread>

unsigned const max_hazard_pointers = 100;
struct hazard_pointer {
    std::atomic<std::thread::id> id;
    std::atomic<void *> pointer;
};
hazard_pointer hazard_pointers[max_hazard_pointers];

class hp_owner {
    hazard_pointer *hp;
public:
    hp_owner(hp_owner const &) = delete;

    hp_owner operator=(hp_owner const &) = delete;

    hp_owner() :
            hp(nullptr) {
        for (unsigned i = 0; i < max_hazard_pointers; ++i) {
            std::thread::id old_id;
            if (hazard_pointers[i].id.compare_exchange_strong(old_id, std::this_thread::get_id())) {        // 6
                hp = &hazard_pointers[i];
                break;                                                                                      // 7
            }
        }
        if (!hp) {                                                                                          // 1
            throw std::runtime_error("No hazard pointers available");
        }
    }

    std::atomic<void *> &get_pointer() {
        return hp->pointer;
    }

    ~hp_owner() {                                                                                            // 2
        hp->pointer.store(nullptr);                                                                         // 8
        hp->id.store(std::thread::id());                                                                     // 9
    }
};

std::atomic<void *> &get_hazard_pointer_for_current_thread() {                                              // 3
    thread_local static hp_owner hazard;                                                                  // 4 每个线程都有自己的风险指针
    return hazard.get_pointer();                                                                            // 5
}

int main() {}

/**
 * get_hazard_pointer_for_current_thread()很简单③：一个hp_owner④类型的thread_local(本线程所有)变量，用来存储当前线程的
 * 风险指针，可以返回这个变量所持有的指针⑤。
 *
 * 第一次有线程调用这个函数时，新hp_owner实例就被创建。这个实例的构造函数⑥会通过查询“所有者/指针”表，寻找没有所有者的
 * 记录。其用compare_exchange_strong()来检查某个记录是否有所有者，并进行析构②。当compare_exchange_strong()失败，其他线程
 * 拥有这个记录，所以可以继续执行。当交换成功，当前线程就拥有了这条记录，而后对其进行存储，并停止搜索⑦。当遍历了列表也没
 * 有找到所有权的记录①，说明有很多线程在使用风险指针，所以这里将抛出一个异常。
 *
 * 当线程退出时，hp_owner的实例将会被销毁。析构函数会在std::thread::id()设置拥有者ID前，将指针重置为nullptr,这样就允许其
 * 他线程对这条记录进行复用⑧⑨。
 */