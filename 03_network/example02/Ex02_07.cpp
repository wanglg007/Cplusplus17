//Reading from a TCP socket asynchronously
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

// Keeps objects we need in a callback to identify whether all data has been read from the socket and to initiate next async
// reading operation if needed.
struct Session {
    std::shared_ptr<asio::ip::tcp::socket> sock;
    std::unique_ptr<char[]> buf;
    std::size_t total_bytes_read;
    unsigned int buf_size;
};

// Function used as a callback for asynchronous reading operation. Checks if all data has been read from the socket and
// initiates new readnig operation if needed.
void callback(const boost::system::error_code &ec, std::size_t bytes_transferred, std::shared_ptr<Session> s) {
    if (ec != 0) {
        std::cout << "Error occured! Error code = "
                  << ec.value()
                  << ". Message: " << ec.message();

        return;
    }

    s->total_bytes_read += bytes_transferred;

    if (s->total_bytes_read == s->buf_size) {
        return;
    }

    s->sock->async_read_some(
            asio::buffer(
                    s->buf.get() +
                    s->total_bytes_read,
                    s->buf_size -
                    s->total_bytes_read),
            std::bind(callback, std::placeholders::_1,
                      std::placeholders::_2, s));
}

void readFromSocket(std::shared_ptr<asio::ip::tcp::socket> sock) {
    std::shared_ptr<Session> s(new Session);

    const unsigned int MESSAGE_SIZE = 7;

    // Step 4. Allocating the buffer.
    s->buf.reset(new char[MESSAGE_SIZE]);

    s->total_bytes_read = 0;
    s->sock = sock;
    s->buf_size = MESSAGE_SIZE;

    // Step 5. Initiating asynchronous reading opration.
    s->sock->async_read_some(
            asio::buffer(s->buf.get(), s->buf_size),
            std::bind(callback,
                      std::placeholders::_1,
                      std::placeholders::_2,
                      s));
}

int main() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;

        // Step 3. Allocating, opening and connecting a socket.
        std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(ios, ep.protocol()));

        sock->connect(ep);

        readFromSocket(sock);

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
 * The following algorithm describes the steps required to implement an application, which reads data from a socket
 * asynchronously. Note that this algorithm provides a possible way to implement such an application. Boost.Asio is quite
 * flexible and allows us to organize and structure the application by reading data from a socket asynchronously in
 * different ways:
 * (1) Define a data structure that contains a pointer to a socket object, a buffer, a variable that defines the size of
 * the buffer, and a variable used as a counter of bytes read.
 * (2) Define a callback function that will be called when an asynchronous reading operation is completed.
 * (3) In a client application, allocate and open an active TCP socket, and then, connect it to a remote application. In
 * a server application, obtain a connected active TCP socket by accepting a connection request.
 * (4) Allocate a buffer big enough for the expected message to fit in.
 * (5) Initiate an asynchronous reading operation by calling the socket's async_read_some() method, specifying a function
 * defined in step 2 as a callback.
 * (6) Call the run() method on an object of the asio::io_service class.
 * (7) In a callback, increase the counter of bytes read. If the number of bytes read is less than the total amount of
 * bytes to be read (that is, the size of an expected message),initiate a new asynchronous reading operation to read the
 * next portion of data.
 *
 * Let's implement a sample client application which will perform asynchronous reading in accordance with the preceding
 * algorithm.
 */

