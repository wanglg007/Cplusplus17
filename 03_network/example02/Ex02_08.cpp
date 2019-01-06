//Canceling asynchronous operations
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// Keeps objects we need in a callback to identify whether all data has been written to the socket and to initiate next async
// writing operatino if needed.
struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::string buf;
};

// Function used as a callback for asynchronous writing operation. Checks if all data from the buffer has been written to
// the socket and initiates new writting operation if needed.
void callback(const boost::system::error_code &ec,
              std::size_t bytes_transferred,
              std::shared_ptr<Session> s) {
    if (ec != 0) {
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();

        return;
    }

    // Here we know that all the data has been written to the socket.
}

void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {

    std::shared_ptr<Session> s(new Session);

    // Step 4. Allocating and filling the buffer.
    s->buf = std::string("Hello");
    s->sock = sock;

    // Step 5. Initiating asynchronous write opration.
    s->sock->async_write_some(
            asio::buffer(s->buf),
            std::bind(callback,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      s));
}

int main() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        asio::ip::tcp::endpoint
                ep(asio::ip::address::from_string(raw_ip_address),
                   port_num);

        asio::io_service ios;

        // Step 3. Allocating, opening and connecting a socket.
        std::shared_ptr<asio::ip::tcp::socket> sock(
                new asio::ip::tcp::socket(ios, ep.protocol()));

        sock->connect(ep);

        writeToSocket(sock);

        // Step 6.
        ios.run();
    }
    catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * The following algorithm provides the steps required to initiate and cancel asynchronous operations with Boost.Asio:
 * (1) If the application is intended to run on Windows XP or Windows Server 2003, define flags that enable asynchronous
 * operation canceling on these versions of Windows.
 * (2) Allocate and open a TCP or UDP socket. It may be an active or passive (acceptor) socket in the client or server
 * application.
 * (3) Define a callback function or functor for an asynchronous operation. If needed, in this callback, implement a branch
 * of code that handles the situation when the operation has been canceled.
 * (4) Initiate one or more asynchronous operations and specify a function or an object defined in step 4 as a callback.
 * (5) Spawn an additional thread and use it to run the Boost.Asio event loop.
 * (6) Call the cancel() method on the socket object to cancel all the outstanding asynchronous operations associated with
 * this socket.
 */

