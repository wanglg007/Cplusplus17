//Performing a stream-based I/O
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    asio::ip::tcp::iostream stream("localhost", "3333");
    if (!stream) {
        std::cout << "Error occurred! Error code = "
                  << stream.error().value()
                  << ". Message = " << stream.error().message()
                  << std::endl;

        return -1;
    }

    stream << "Request.";
    stream.flush();

    std::cout << "Response: " << stream.rdbuf();

    return 0;
}

/**
 * The concepts of a stream and stream-based I/O are powerful in their expressiveness and elegance when used properly.
 * Sometimes, most of the application's source code consists of stream-based I/O operations. The source code readability
 * and maintainability of such an application would be increased if network communication modules were implemented by
 * means of stream-based operations as well.
 */

