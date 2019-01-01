//
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>

int main(void) {
    boost::asio::io_service io_svc;
    boost::shared_ptr<boost::asio::io_service::work> worker(
            new boost::asio::io_service::work(io_svc)
    );

    worker.reset();

    io_svc.run();

    std::cout << "We will not see this line in console window :(" << std::endl;

    return 0;
}

/**
 * We can also unblock the program by removing the work object from the io_service object, but we have to use a pointer to
 * the work object in order to remove the work object itself. We are going to use the shared_ptr pointer, a smart pointer
 * provided by the Boost libraries.
 */