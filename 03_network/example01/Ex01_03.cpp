//Creating a passive socket
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // Step 1. An instance of 'io_service' class is required by socket constructor.
    asio::io_service ios;

    // Step 2. Creating an object of 'tcp' class representing a TCP protocol with IPv6 as underlying protocol.
    asio::ip::tcp protocol = asio::ip::tcp::v6();

    // Step 3. Instantiating an acceptor socket object.
    asio::ip::tcp::acceptor acceptor(ios);

    // Used to store information about error that happens while opening the acceptor socket.
    boost::system::error_code ec;

    // Step 4. Opening the acceptor socket.
    acceptor.open(protocol, ec);

    if (ec.value() != 0) {
        // Failed to open the socket.
        std::cout
                << "Failed to open the acceptor socket!"
                << "Error code = "
                << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    return 0;
}

/**
 * In Boost.Asio a passive socket is represented by the asio::ip::tcp::acceptor class.The name of the class suggests the
 * key function of the objects of the class—to listen for and accept or handle incoming connection requests.The following
 * algorithm describes the steps required to perform to create an acceptor socket:
 * (1) Create an instance of the asio::io_service class or use the one that has been created earlier.
 * (2) Create an object of the asio::ip::tcp class that represents the TCP protocol and the required version of the
 * underlying IP protocol (IPv4 or IPv6).
 * (3) Create an object of the asio::ip::tcp::acceptor class representing an acceptor socket, passing the object of the
 * asio::io_service class to its constructor.
 * (4) Call the acceptor socket's open() method, passing the object representing the protocol created in step 2 as an argument.
 */
