//Using timers
#include <boost/asio/steady_timer.hpp>
#include <iostream>

using namespace boost;

int main() {
    asio::io_service ios;

    asio::steady_timer t1(ios);
    t1.expires_from_now(std::chrono::seconds(2));

    asio::steady_timer t2(ios);
    t2.expires_from_now(std::chrono::seconds(5));

    t1.async_wait([&t2](boost::system::error_code ec) {
        if (ec == 0) {
            std::cout << "Timer #2 has expired!" << std::endl;
        } else if (ec == asio::error::operation_aborted) {
            std::cout << "Timer #2 has been cancelled!" << std::endl;
        } else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message() << std::endl;
        }

        t2.cancel();
    });

    t2.async_wait([](boost::system::error_code ec) {
        if (ec == 0) {
            std::cout << "Timer #2 has expired!" << std::endl;
        } else if (ec == asio::error::operation_aborted) {
            std::cout << "Timer #2 has been cancelled!" << std::endl;
        } else {
            std::cout << "Error occured! Error code = "
                      << ec.value()
                      << ". Message: " << ec.message() << std::endl;
        }
    });

    ios.run();

    return 0;
}

/**
 * Timing is a very important aspect of software systems in general and distributed applications in particular. Therefore
 * a hardware timer—a device used to measure time intervals—is essential component of any computer and all modern operating
 * systems provide interface allowing applications to use it.
 * There are two typical use cases related to the timer. The first one assumes that the application wants to know the current
 * time and asks the operating system to find it out.
 * The second use case is when the application asks the operating system to notify it (usually,by means of invoking a callback)
 * when a certain amount of time elapses.
 * The second use case is particularly important when it comes to developing distributed applications with Boost.Asio because
 * a timer is the only way to implement the timeout mechanism for asynchronous operations.
 *
 * The Boost.Asio library includes several classes that implement timers, which we will consider in this recipe.
 */

