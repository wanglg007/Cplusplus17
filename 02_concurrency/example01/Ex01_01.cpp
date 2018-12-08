//(1.1)使用互斥量保护列表
#include <list>
#include <mutex>
#include <algorithm>
#include <iostream>

std::list<int> some_list;                                   // 1
std::mutex some_mutex;                                      // 2

void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex);          // 3
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);          // 4
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

int main() {
    add_to_list(42);
    std::cout << "contains(1)=" << list_contains(1) << ",contains(42)=" << list_contains(42) << std::endl;
}

/**
 * C++标准库为互斥量提供了一个RAII语法的模板类std::lack_guard，其会在构造的时候提供已锁的互斥量，并在析构的时候进行解锁，从而保证了一个已锁的互斥量总是会被正确的解锁。
 *
 * 程序清单中展示了如何在多线程程序中使用std::mutex构造的std::lock_guard实例对一个列表进行访问保护。
 *
 * 清单中有一个全局变量①，这个全局变量被一个全局的互斥量保护②。add_to_list()③和list_contains()④函数中使用std::lock_guard<std::mutex>，使得这两个函数中对数据的访问
 * 是互斥的：list_contains()不可能看到正在被add_to_list()修改的列表。
 *
 * 面向对象设计的准则：函数add_to_list和list_contains可以作为类的成员函数。互斥量和要保护的数据在类中都需要定义为private成员，这会让访问数据的代码变的清晰，并且容易看
 * 出在什么时候对互斥量上锁。当所有成员函数都会在调用时对数据上锁，结束时对数据解锁，那么就保证了数据访问时不变量不被破坏。
 */