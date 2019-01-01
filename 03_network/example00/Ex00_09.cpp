//
#include <boost/asio.hpp>
#include <iostream>

int main(void) {
    boost::asio::io_service io_svc;

    io_svc.run();

    std::cout << "We will see this line in console window." << std::endl;

    return 0;
}

/**
 * In programming, event is an action or occurrence detected by a program, which will be handled by the program using the
 * event handler object. The io_service object has one or more instances where events are handled, which is event processing loop.
 *
 *
 */
