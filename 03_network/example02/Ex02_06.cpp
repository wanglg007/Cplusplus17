//Writing to a TCP socket asynchronously
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// Step 1:Keeps objects we need in a callback to identify whether all data has been written to the socket and to initiate next
// async writing operatino if needed.
struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::string buf;
    std::size_t total_bytes_written;
};

// Step 2:Function used as a callback for asynchronous writing operation. Checks if all data from the buffer has been written to
// the socket and initiates new writting operation if needed.
void callback(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<Session> s) {
    if (ec != 0) {
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();

        return;
    }

    s->total_bytes_written += bytes_transferred;

    if (s->total_bytes_written == s->buf.length()) {
        return;
    }

    s->sock->async_write_some(
            asio::buffer(
                    s->buf.c_str() +
                    s->total_bytes_written,
                    s->buf.length() -
                    s->total_bytes_written),
            std::bind(callback, std::placeholders::_1,
                      std::placeholders::_2, s));
}

//Let's skip step 3 for now and implement steps 4 and 5 in a separate function. Let's call this function writeToSocket():
void writeToSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {

    std::shared_ptr<Session> s(new Session);

    // Step 4. Allocating and filling the buffer.
    s->buf = std::string("Hello");
    s->total_bytes_written = 0;
    s->sock = sock;

    // Step 5. Initiating asynchronous write opration.
    s->sock->async_write_some(
            asio::buffer(s->buf),
            std::bind(callback,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      s));
}

//Now, we come back to step 3 and implement it in the main()application entry point function:
int main() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;

        // Step 3. Allocating, opening and connecting a socket.
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(ios, ep.protocol()));

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
 * The following algorithm describes the steps required to perform and implement an application, which writes data to a
 * TCP socket asynchronously. Note that this algorithm provides a possible way to implement such an application. Boost.Asio
 * is quite flexible and allows us to organize and structure the application by writing data to a socket asynchronously
 * in many different ways:
 * (1) Define a data structure that contains a pointer to a socket object, a buffer, and a variable used as a counter of
 * bytes written.
 * (2) Define a callback function that will be called when the asynchronous writing operation is completed.
 * (3) In a client application, allocate and open an active TCP socket and connect it to a remote application. In a server
 * application, obtain a connected active TCP socket by accepting a connection request.
 * (4) Allocate a buffer and fill it with data that is to be written to the socket.
 * (5) Initiate an asynchronous writing operation by calling the socket's async_write_some() method. Specify a function
 * defined in step 2 as a callback.
 * (6) Call the run() method on an object of the asio::io_service class.
 * (7) In a callback, increase the counter of bytes written. If the number of bytes written is less than the total amount
 * of bytes to be written, initiate a new asynchronous writing operation to write the next portion of the data.
 */

