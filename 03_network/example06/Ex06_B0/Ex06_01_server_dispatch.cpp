//
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
io_service service;

void func(int i) {
    std::cout << "func called, i= " << i << std::endl;
}

void run_dispatch_and_post() {
    for (int i = 0; i < 10; i += 2) {
        //当前的线程调用了service.run()， 它可以在方法中直接调用handler。
        service.dispatch(boost::bind(func, i));
        //确保其在请求io_service实例， 然后调用指定的处理方法之后立即返回。 handler稍后会在某个调用了service.run()的线程中被调用。
        service.post(boost::bind(func, i + 1));

    }
}

int main(int argc, char *argv[]) {
    service.post(run_dispatch_and_post);
    service.run();
}


