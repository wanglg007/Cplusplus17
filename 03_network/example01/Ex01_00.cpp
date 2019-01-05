//Creating an endpoint in the client to designate the server
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // Step1. Assume that the client application has already obtained the IP-address and the protocol port number.
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    // Used to store information about error that happens while parsing the raw IP-address.
    boost::system::error_code ec;

    // Step2. Using IP protocol version independent address representation.
    asio::ip::address ip_address = asio::ip::address::from_string(raw_ip_address, ec);

    if (ec.value() != 0) {
        // Provided IP address is invalid. Breaking execution.
        std::cout
                << "Failed to parse the IP address. Error code = "
                << ec.value() << ". Message: " << ec.message();
        return ec.value();
    }

    // Step 3.
    asio::ip::tcp::endpoint ep(ip_address, port_num);

    // Step 4. The endpoint is ready and can be used to specify a particular server in the network the client wants to
    // communicate with.

    return 0;
}

/**
 * The following algorithm describes steps required to perform in the client application to create an endpoint designating
 * a server application the client wants to communicate with. Initially,the IP address is represented as a string in the
 * dot-decimal notation if this is an IPv4 address or in hexadecimal notation if this is an IPv6 address:
 * (1) Obtain the server application's IP address and port number. The IP address should be specified as a string in the
 * dot-decimal (IPv4) or hexadecimal (IPv6) notation.
 * (2) Represent the raw IP address as an object of the asio::ip::address class.
 * (3) Instantiate the object of the asio::ip::tcp::endpoint class from the address object created in step 2 and a port number.
 * (4) The endpoint is ready to be used to designate the server application in Boost.Asio communication related methods.
 *
 */

