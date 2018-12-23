//
#include <thread>
#include <utility>

class scoped_thread {
    std::thread t;
public:
    explicit scoped_thread(std::thread t_) : t(std::move(t_)) {
        if (!t.joinable()) {
            throw std::logic_error("No thread");
        }
    }

    ~scoped_thread() {
        t.join();
    }

    scoped_thread(scoped_thread const &) = delete;

    scoped_thread &operator=(scoped_thread const &) = delete;
};

void do_something(int &i) {
    ++i;
}

struct func {
    int &i;

    func(int &i_) : i(i_) {

    }

    void operator()() {
        for (unsigned j = 0; j < 100000; ++j) {
            do_something(i);
        }
    }
};

void do_something_in_current_thread() {

}

void f() {
    int some_local_state;
    scoped_thread t(std::thread(func(some_local_state)));

    do_something_in_current_thread();
}

int main() {
    f();
}

/**
 * 若需要后台启动线程的函数，则可以通过新线程返回的所有权去调用该函数，而不是等待线程结束再去调用；总之，新线程的所有权都
 * 需要转移。
 *
 * 下面示例创建了两个执行线程，并且在std::thread实例之间(t1,t2和t3)转移所有权：
 *  void some_function();
 *  void some_other_function();
 *  std::thread t1(some_function);          // 1
 *  std::thread t2=std::move(t1);           // 2
 *  t1=std::thread(some_other_function);    // 3
 *  std::thread t3;                         // 4
 *  t3=std::move(t2);                       // 5
 *  t1=std::move(t3);                       // 6 赋值操作将使程序崩溃
 * 当显式使用std::move()创建t2后②，t1的所有权就转移给了t2。之后t1和执行线程已经没有关联；执行some_function的函数现在与t2
 * 关联。然后与临时std::thread对象相关的线程启动③。t3使用默认构造方式创建④，与任何执行线程都没有关联。调用std::move()将
 * 与t2关联线程的所有权转移到t3中⑤，显式的调用std::move()是因为t2是个命名对象。移动操作⑤完成后，t1与执行some_other_function
 * 的线程相关联，t2与任何线程都无关联，t3与执行some_function的线程相关联。
 * 最后一个移动操作将执行some_function线程的所有权转移⑥给t1。这时t1已经有一个关联的线(执行some_other_function的线程)，所以
 * 可以直接调用std::terminate()终止程序继续运行。终止操作将调用std::thread的析构函数销毁所有对象。
 *
 * std::thread支持移动意味着线程的所有权可以在函数外进行转移，就如下面程序一样：
 * std::thread f()
 * {
 *   void some_function();
 *   return std::thread(some_function);
 *  }
 *  std::thread g()
 * {
 *  void some_other_function(int);
 *  std::thread t(some_other_function,42);
 *  return t;
 *  }
 *
 * 当所有权可以在函数内部传递，就允许std::thread实例可作为参数进行传递，代码如下：
 *  void f(std::thread t);
 *  void g()
 *  {
 *   void some_function();
 *   f(std::thread(some_function));
 *   std::thread t(some_function);
 *   f(std::move(t));
 *  }
 *
 * std::thread支持移动的好处是可以创建thread_guard类的实例，并且拥有其线程的所有权。当thread_guard对象所持有的线程已经被引用，
 * 移动操作就可以避免很多麻烦；这意味着当某个对象转移线程的所有权后，它不能对线程进行加入或分离。为了确保线程程序退出前完成，下
 * 面的代码里定义了scoped_thread类。现在来看一下这段代码：
 *  class scoped_thread
 *  {
 *   std::thread t;
 *   public:
 *   explicit scoped_thread(std::thread t_):         // 1
 *   t(std::move(t_))
 *   {
 *   if(!t.joinable())                               // 2
 *   throw std::logic_error(“No thread”);
 *   }
 *   ~scoped_thread()
 *   {
 *   t.join();                                       // 3
 *   }
 *   scoped_thread(scoped_thread const&)=delete;
 *   scoped_thread& operator=(scoped_thread const&)=delete;
 *   };
 *   struct func;                                   //定义在清单中
 *   void f()
 *   {
 *   int some_local_state;
 *   scoped_thread t(std::thread(func(some_local_state))); // 4
 *   do_something_in_current_thread();
 *   }
 * 新线程是直接传递到scoped_thread中④，而非创建一个独立的命名变量。当主线程到达f()函数的末尾时，scoped_thread对象将会销毁，
 * 然后加入③到的构造函数①创建的线程对象中。而在清单中的thread_guard类就要在析构的时候检查线程是否可加入。这里把检查放在
 * 构造函数中②，并且当线程不可加入时，抛出异常。
 *
 *  下述代码量产线程，并且等待它们结束。
 *  void do_work(unsigned id);
 *  void f()
 *  {
 *   std::vector<std::thread> threads;
 *   for(unsigned i=0; i < 20; ++i)
 *   {
 *   threads.push_back(std::thread(do_work,i));                                     // 产生线程
 *   }
 *   std::for_each(threads.begin(),threads.end(),std::mem_fn(&std::thread::join)); // 对每个线程调用join()
 *   }
 *  将std::thread放入std::vector是向线程自动化管理迈出的第一步。
 */