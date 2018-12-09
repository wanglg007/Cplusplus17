//使用一个互斥量的延迟初始化(线程安全)过程
#include <memory>
#include <mutex>

struct some_resource {
    void do_something() {

    }
};

std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;

void foo() {
    std::unique_lock<std::mutex> lk(resource_mutex);    // 所有线程在此序列化
    if (!resource_ptr) {
        resource_ptr.reset(new some_resource);          // 1 只有初始化过程需要保护
    }
    lk.unlock();
    resource_ptr->do_something();
}

int main() {
    foo();
}

/**
 * 当共享数据对于并发访问是安全的，①是转为多线程代码时需要保护的，但是下面天真的转换会使得线程资源产生不必要的序列化。这是因为每个线程必须等
 * 待互斥量，为了确定数据源已经初始化。
 *
 *  void undefined_behaviour_with_double_checked_locking()
 *  {
 *   if(!resource_ptr)      // 1
 *   {
 *   std::lock_guard<std::mutex> lk(resource_mutex);
 *   if(!resource_ptr)      // 2
 *   {
 *   resource_ptr.reset(new some_resource);     // 3
 *   }
 *   }
 *   resource_ptr->do_something();              // 4
 *   }
 * 指针第一次读取数据不需要获取锁①，并且只有在指针为NULL时才需要获取锁。然后当获取锁之后，指针会被再次检查一遍② (这就是双重检查的部分)，避免另一的线程在第一次检查
 * 后再做初始化，并且让当前线程获取锁。
 * 这个模式为什么声名狼藉呢？因为这里有潜在的条件竞争，因为外部的读取锁①没有与内部的写入锁进行同步③。因此就会产生条件竞争，这个条件竞争不仅覆盖指针本身，还会影响到
 * 其指向的对象；即使一个线程知道另一个线程完成对指针进行写入，它可能没有看到新创建的some_resource实例，然后调用do_something()④后，得到不正确的结果。
 *
 *  std::shared_ptr<some_resource> resource_ptr;
 *  std::once_flag resource_flag;           // 1
 *  void init_resource()
 *  {
 *   resource_ptr.reset(new some_resource);
 *  }
 *  void foo()
 *  {
 *   std::call_once(resource_flag,init_resource); // 可以完整的进行一次初始化
 *   resource_ptr->do_something();
 *  }
 * 例子中，std::once_flag ①和初始化好的数据都是命名空间区域的对象，但是std::call_once()可仅作为延迟初始化的类型成员
 */