//使用 std::future 从异步任务中获取返回值
#include <future>
#include <iostream>

int find_the_answer_to_ltuae() {
    return 42;
}

void do_other_stuff() {

}

int main() {
    std::future<int> the_answer = std::async(find_the_answer_to_ltuae);
    do_other_stuff();
    std::cout << "The answer is " << the_answer.get() << std::endl;
}

/**
 * 可以使用std::async启动一个异步任务。与std::thread对象等待运行方式的不同，std::async会返回一个std::future对象，这个对象持有最终计算
 * 的结果。当需要这个值时，只需要调用这个对象的get()成员函数；并且直到“期望”状态为就绪的情况下，线程才会阻塞；之后，返回计算结果。
 */
