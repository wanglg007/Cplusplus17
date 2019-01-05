//Binding a socket to an endpoint - tcp
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // Step 1. Here we assume that the server application has already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Creating an endpoint.
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

    // Used by 'acceptor' class constructor.
    asio::io_service ios;

    // Step 3. Creating and opening an acceptor socket.
    asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

    boost::system::error_code ec;

    // Step 4. Binding the acceptor socket.
    acceptor.bind(ep, ec);

    // Handling errors if any.
    if (ec != 0) {
        // Failed to bind the acceptor socket. Breaking execution.
        std::cout << "Failed to bind the acceptor socket."
                  << "Error code = " << ec.value() << ". Message: "
                  << ec.message();

        return ec.value();
    }

    return 0;
}

/**
 * The following algorithm describes steps required to create an acceptor socket and to bind it to an endpoint designating
 * all IP addresses available on the host and a particular protocol port number in the IPv4 TCP server application:
 * (1) Obtain the protocol port number on which the server should listen for incoming connection requests.
 * (2) Create an endpoint that represents all IP addresses available on the host and the protocol port number obtained in the step 1.
 * (3) Create and open an acceptor socket.
 * (4) Call the acceptor socket's bind() method, passing the endpoint object as an argument to it.
 */