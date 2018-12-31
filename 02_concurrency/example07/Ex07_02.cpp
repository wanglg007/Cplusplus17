//可等待任务的线程池
#include <deque>
#include <future>
#include <memory>
#include <functional>
#include <iostream>
#include <iostream>

class function_wrapper {
    struct impl_base {
        virtual void call() = 0;

        virtual ~impl_base() {}
    };

    std::unique_ptr<impl_base> impl;

    template<typename F>
    struct impl_type : impl_base {
        F f;

        impl_type(F &&f_) : f(std::move(f_)) {}

        void call() { f(); }
    };

public:
    template<typename F>
    function_wrapper(F &&f):
            impl(new impl_type<F>(std::move(f))) {}

    void call() { impl->call(); }

    function_wrapper(function_wrapper &&other) :
            impl(std::move(other.impl)) {}

    function_wrapper &operator=(function_wrapper &&other) {
        impl = std::move(other.impl);
        return *this;
    }

    function_wrapper(const function_wrapper &) = delete;

    function_wrapper(function_wrapper &) = delete;

    function_wrapper &operator=(const function_wrapper &) = delete;
};

class thread_pool {
public:
    std::deque<function_wrapper> work_queue;                                     // Use function_wrapper rather than std::function

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>                  // 1
    submit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;     // 2

        std::packaged_task<result_type()> task(std::move(f));                    // 3
        std::future<result_type> res(task.get_future());                         // 4
        work_queue.push_back(std::move(task));                                   // 5
        return res;                                                             // 6
    }
    // rest as before
};

/**
 * First, the modified submit() function (1) returns a std::future<> to hold the return value of the task and allow the
 * caller to wait for the task to complete. This requires that you know the return type of the supplied function f, which
 * is where std::result_of<> comes in: std::result_of<FunctionType()>::type is the type of the result of invoking an
 * instance of type FunctionType (such as f) with no arguments.You use the same std::result_of<> expression for the
 * result_type typedef (2) inside the function.
 *
 * You then wrap the function f in a std::packaged_task<result_type()> (3),because f is a function or callable object that
 * takes no parameters and returns an instance of type result_type, as we just deduced. You can now get your future from
 * the std::packaged_task<> (4), before pushing the task onto the queue (5) and returning the future (6). Note that you have
 * to use std::move() when pushing the task onto the queue, because std::packaged_task<> isn’t copyable. The queue now
 * stores function_wrapper objects rather than std::function<void()> objects in order to handle this.
 *
 */