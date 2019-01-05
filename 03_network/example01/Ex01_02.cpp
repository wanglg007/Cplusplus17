//Creating an active socket
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // Step 1. An instance of 'io_service' class is required by socket constructor.
    asio::io_service ios;

    // Step 2. Creating an object of 'tcp' class representing a TCP protocol with IPv4 as underlying protocol.
    asio::ip::tcp protocol = asio::ip::tcp::v4();
    // Step 2. Creating an object of 'udp' class representing a UDP protocol with IPv6 as underlying protocol.
    // asio::ip::udp protocol = asio::ip::udp::v6();

    // Step 3. Instantiating an active TCP socket object.
    asio::ip::tcp::socket sock(ios);

    // Used to store information about error that happens while opening the socket.
    boost::system::error_code ec;

    // Step 4. Opening the socket.
    sock.open(protocol, ec);

    if (ec.value() != 0) {
        // Failed to open the socket.
        std::cout
                << "Failed to open the socket! Error code = "
                << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    return 0;
}

/**
 * The following algorithm describes the steps required to perform in a client application to create and open an active
 * socket:
 * (1) Create an instance of the asio::io_service class or use the one that has been created earlier.
 * (2) Create an object of the class that represents the transport layer protocol (TCP or UDP) and the version of the
 * underlying IP protocol (IPv4 or IPv6) over which the socket is intended to communicate.
 * (3) Create an object representing a socket corresponding to the required protocol type.Pass the object of asio::io_service
 * class to the socket's constructor.
 * (4) Call the socket's open() method, passing the object representing the protocol created in step 2 as an argument.
 *
 */