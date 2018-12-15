//使用 std::packaged_task 执行一个图形界面线程
#include <deque>
#include <mutex>
#include <future>
#include <thread>
#include <utility>

std::mutex m;
std::deque<std::packaged_task<void()>> tasks;

bool gui_shutdown_message_received();

void get_and_process_gui_message();

void gui_thread() {                             // 1
    while (!gui_shutdown_message_received()) {  // 2
        get_and_process_gui_message();           // 3
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lk(m);
            if (tasks.empty())                  // 4
                continue;
            task = std::move(tasks.front());    // 5
            tasks.pop_front();
        }
        task();                                 // 6
    }
}

std::thread gui_bg_thread(gui_thread);

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f) {
    std::packaged_task<void()> task(f);         // 7
    std::future<void> res = task.get_future();  // 8
    std::lock_guard<std::mutex> lk(m);          // 9
    tasks.push_back(std::move(task));           // 10
    return res;
}

int main() {

}

/**
 * 图形界面线程①循环直到收到一条关闭图形界面的信息后关闭②，进行轮询界面消息处理③，例如用户点击，和执行在队列中的任务。当队列中没有任务④，它将再次
 * 循环；除非他能在队列中提取出一个任务⑤，然后释放队列上的锁，并且执行任务⑥。这里“期望”与任务相关，当任务执行完成时，其状态会被置为“就绪”状态。
 *
 * 将一个任务传入队列，也很简单：提供的函数⑦可以提供一个打包好的任务，可以通过这个任务⑧调用get_future()成员函数获取“期望”对象，并且在任务被推入列
 * 表⑨之前，“期望”将返回调用函数⑩。当需要知道线程执行完任务时，向图形界面线程发布消息的代码，会等待“期望”改变状态；否则，则会丢弃这个“期望”。
 *
 * 例子使用std::packaged_task<void()>创建任务，其包含了一个无参数无返回值的函数或可调用对象(如果当这个调用有返回值时，返回值会被丢弃)。
 */