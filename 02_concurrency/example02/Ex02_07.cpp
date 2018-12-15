//使用 std::async 向函数传递参数
#include <string>
#include <future>

struct X {
    void foo(int, std::string const &);

    std::string bar(std::string const &);
};

X x;

auto f1 = std::async(&X::foo, &x, 42, "hello"); // 调用p->foo(42, "hello")
auto f2 = std::async(&X::bar, x, "googbye");    // 调用tmpx.bar("goodbye")

struct Y {
    double operator()(double);
};

Y y;

auto f3 = std::async(Y(), 3.141);               // 调用tmpy(3.141)，tmpy通过Y的移动构造函数
auto f4 = std::async(std::ref(y), 2.718);       // 调用y(2.718)

X baz(X &);

auto f6 = std::async(baz, std::ref(x));         // 调用baz(x)

class move_only {
public:
    move_only();

    move_only(move_only &&);

    move_only(move_only const &) = delete;

    move_only &operator=(move_only &&);

    move_only &operator=(move_only const &) = delete;

    void operator()();
};

auto f5 = std::async(move_only());              // 调用tmp()，tmp是通过std::move(move_only())

int main() {

}

/**
 * std::async允许通过添加额外的调用参数向函数传递额外的参数。第一个参数是一个指向成员函数的指针，第二个参数提供有这个函数成员类的具体对象，剩余的参数可作为成员
 * 函数的参数传入。否则，第二个和随后的参数将作为函数的参数，或作为指定可调用对象的第一个参数。就如std::thread，当参数为右值(rvalues)时，拷贝操作将使用移动的方
 * 式转移原始数据。这就允许使用“只移动”类型作为函数对象和参数。
 *
 * 默认情况下，取决于std::async是否启动一个线程，或是否在期望等待时同步任务。也可以在函数调用之前，向std::async传递一个额外参数。这个参数的类型是std::launch，还可
 * 以是std::launch::defered，用来表明函数调用被延迟到wait()或get()函数调用时才执行，std::launch::async表明函数必须在其所在的独立线程上执行，std::launch::deferred |
 * std::launch::async表明实现可以选择这两种方式的一种。最后一个选项是默认的。当函数调用被延迟，它可能不会在运行。
 *
 * auto f6=std::async(std::launch::async,Y(),1.2);              // 在新线程上执行
 * auto f7=std::async(std::launch::deferred,baz,std::ref(x));   // 在wait()或get()调用时执行
 * auto f8=std::async(std::launch::deferred | std::launch::async,baz,std::ref(x));  // 实现选择执行方式
 * auto f9=std::async(baz,std::ref(x));
 * f7.wait();                                                   // 调用延迟函数
 *
 * 使用std::async会让分割算法到各个任务中变的容易，这样程序就能并发的执行。不过这不是让一个std::future与一个任务实例相关联的唯一方式；也可以将任务包装入一个
 * std::packaged_task<> 实例中，或通过编写代码的方式，使用std::promise<>类型模板显示设置值。
 */