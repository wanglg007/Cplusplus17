//Writing to a TCP socket synchronously
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

void writeToSocketEnhanced(asio::ip::tcp::socket &sock) {
    // Allocating and filling the buffer.
    std::string buf = "Hello";
    // Write whole buffer to the socket.
    asio::write(sock, asio::buffer(buf));
}

void writeToSocket(asio::ip::tcp::socket &sock) {
    // Allocating and filling the buffer.
    std::string buf = "Hello";

    std::size_t total_bytes_written = 0;

    // Run the loop until all data is written to the socket.
    while (total_bytes_written != buf.length()) {
        total_bytes_written += sock.write_some(asio::buffer(buf.c_str() +
                                                            total_bytes_written,
                                                            buf.length() - total_bytes_written));
    }
}

int main() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        asio::ip::tcp::endpoint
                ep(asio::ip::address::from_string(raw_ip_address),
                   port_num);

        asio::io_service ios;

        asio::ip::tcp::socket sock(ios, ep.protocol());

        sock.connect(ep);

        writeToSocket(sock);
    } catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * The following algorithm describes the steps required to synchronously write data to a TCP socket in a distributed
 * application:
 * (1) In a client application, allocate, open, and connect an active TCP socket. In a server application, obtain a
 * connected active TCP socket by accepting a connection request using an acceptor socket.
 * (2) Allocate the buffer and fill it with data that is to be written to the socket.
 * (3) In a loop, call the socket's write_some() method as many times as it is needed to send all the data available
 * in the buffer.
 * The following code sample demonstrates a client application, which operates according to the algorithm.
 */

