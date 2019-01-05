//Creating the server endpoint
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // Step 1. Here we assume that the server application has already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Create special object of asio::ip::address class that specifies all IP-addresses available on the host. Note
    // that here we assume that server works over IPv6 protocol.
    asio::ip::address ip_address = asio::ip::address_v6::any();

    // Step 3.
    asio::ip::tcp::endpoint ep(ip_address, port_num);

    // Step 4. The endpoint is created and can be used to specify the ip-addresses and a port number on which the server
    // application wants to listen to incoming connections.

    return 0;
}

/**
 * The following algorithm describes steps required to perform in a server application to create an endpoint specifying
 * all IP addresses available on the host and a port number on which the server application wants to listen for incoming
 * messages from the clients:
 * (1) Obtain the protocol port number on which the server will listen for incoming requests.
 * (2) Create a special instance of the asio::ip::address object representing all IP addresses available on the host running the server.
 * (3) Instantiate an object of the asio::ip::tcp::endpoint class from the address object created in step 2 and a port number.
 * (4) The endpoint is ready to be used to specify to the operating system that the server wants to listen for incoming
 * messages on all IP addresses and a particular protocol port number.
 *
 */



