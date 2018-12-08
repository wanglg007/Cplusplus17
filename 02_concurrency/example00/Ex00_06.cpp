//(0.6)向线程函数传递参数
#include <thread>

void some_function() {

}

void some_other_function(int) {

}

std::thread f() {
    void some_function();
    return std::thread(some_function);
}

std::thread g() {
    void some_other_function(int);
    std::thread t(some_other_function, 42);
    return t;
}

int main() {
    std::thread t1 = f();
    t1.join();

    std::thread t2 = g();
    t2.join();
}

/**
 * (1)向std::thread构造函数中的可调用对象或函数传递一个参数很简单。默认参数要拷贝到线程独立内存中，即使参数是引用也可以在新线程中进行访问。
 *
 * (2)当指向动态变量的指针作为参数传递给线程的情况，代码如下：
 *  void f(int i,std::string const& s);
 *  void oops(int some_param)
 *  {
 *   char buffer[1024];                      // 1
 *   sprintf(buffer, "%i",some_param);
 *   std::thread t(f,3,buffer);              // 2
 *   t.detach();
 *  }
 * 该情况下：buffer②是一个指针变量，指向本地变量，然后本地变量通过buffer传递到新线程中②。并且函数有很大的可能会在字面值转化成std::string对象之前崩溃，从
 * 而导致线程的未定义行为。解决方案就是在传递到std::thread构造函数之前就将字面值转化为std::string 对象。
 *  void f(int i,std::string const& s);
 *  void not_oops(int some_param)
 *  {
 *   char buffer[1024];
 *   sprintf(buffer,"%i",some_param);
 *   std::thread t(f,3,std::string(buffer)); // 使用std::string，避免悬垂指针
 *   t.detach();
 *  }
 * 该情况下的问题是：若要依赖隐式转换将字面值转换为函数期待的std::string对象，但因std::thread的构造函数会复制变量，就复制没有转换成期望类型的字符串字面值。
 * 成功的传递引用会发生在线程更新数据结构时。
 *  void update_data_for_widget(widget_id w,widget_data& data);     // 1
 *  void oops_again(widget_id w)
 *  {
 *   widget_data data;
 *   std::thread t(update_data_for_widget,w,data);                  // 2
 *   display_status();
 *   t.join();
 *   process_widget_data(data);                                     // 3
 *  }
 *  虽然update_data_for_widget①的第二个参数期待传入一个引用，但是std::thread 的构造函数②并不知晓；构造函数无视函数期待的参数类型，并盲目的拷贝已提供的变量。当线程调用
 *  update_data_for_widget函数时，传递给函数的参数是data变量内部拷贝的引用，而非数据本身的引用。因此当线程结束时，内部拷贝数据将会在数据更新阶段被销毁，且process_widget_data将
 *  会接收到没有修改的data变量③。使用std::bind可以解决这个问题，使用std::ref将参数转换成引用的形式。该情况可将线程的调用改成以下形式：
 *  std::thread t(update_data_for_widget,w,std::ref(data));
 *  之后，update_data_for_widget就会接收到一个data变量的引用，而非一个data变量拷贝的引用。
 *
 *  (3)std::thread构造函数和std::bind的操作都可以传递一个成员函数指针作为线程函数，并提供一个合适的对象指针作为第一个参数：
 *  class X
 *  {
 *   public:
 *   void do_lengthy_work();
 *   };
 *  X my_x;
 *  std::thread t(&X::do_lengthy_work,&my_x);                       // 1
 *  新线程将my_x.do_lengthy_work()作为线程函数；my_x的地址①作为指针对象提供给函数。
 *  class X
 *  {
 *   public:
 *   void do_lengthy_work(int);
 *  };
 *  X my_x;
 *  int num(0);
 *  std::thread t(&X::do_lengthy_work, &my_x, num);
 *  提供的参数可以移动(move)，但不能拷贝(copy)。移动是指:原始对象中的数据转移给另一对象，而转移的这些数据就不再在原始对象中保存。std::unique_ptr就是这样类型，该类型
 *  为动态分配的对象提供内存自动管理机制。同一时间内只允许一个std::unique_ptr实现指向一个给定对象，并且当这个实现销毁时，指向的对象也将被删除。移动构造函数(move constructor)
 *  和移动赋值操作符(move assignment operator)允许一个对象在多个std::unique_ptr实现中传递。使用"移动"转移原对象后会留下一个空指针(NULL)。移动操作可以将对象转换成可接受的
 *  类型。下面的代码展示std::move是如何转移一个动态对象到一个线程中：
 *  void process_big_object(std::unique_ptr<big_object>);
 *  std::unique_ptr<big_object> p(new big_object);
 *  p->prepare_data(42);
 *  std::thread t(process_big_object,std::move(p));
 * 在std::thread的构造函数中指定std::move(p),big_object对象的所有权就被首先转移到新创建线程的的内部存储，之后传递给process_big_object函数。std::unique_ptr和std::thread
 * 在所属权上有相似语义类型。虽然std::thread实例不会如std::unique_ptr去占有一个动态对象所有权，但是它会占用一部分资源的所有权：每个实例都管理一个执行线程。std::thread所有权可以
 * 在多个实例中互相转移，因为这些实例是可移动(movable)且不可复制(aren't copyable)。在同一时间点，就能保证只关联一个执行线程；同时，也允许程序员能在不同的对象之间转移所有权。
 *
 */