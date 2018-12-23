//(0.8)生成一批线程并等待它们完成
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>

void do_work(unsigned id) {
}

void f() {
    std::vector<std::thread> threads;
    for (unsigned i = 0; i < 20; i++) {
        threads.push_back(std::thread(do_work, i));                 //生成线程
    }
    //轮流在每个线程上调用join()
    std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

int main() {
    f();
}

