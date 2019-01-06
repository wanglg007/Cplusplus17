//Reading from a TCP socket synchronously
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

std::string readFromSocketDelim(asio::ip::tcp::socket &sock) {
    asio::streambuf buf;

    // Synchronously read data from the socket until '\n' symbol is encountered.
    asio::read_until(sock, buf, '\n');

    std::string message;

    // Because buffer 'buf' may contain some other data after '\n' symbol, we have to parse the buffer and extract only
    // symbols before the delimiter.
    std::istream input_stream(&buf);
    std::getline(input_stream, message);

    return message;
}

std::string readFromSocketEnhanced(asio::ip::tcp::socket &sock) {
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];

    asio::read(sock, asio::buffer(buf, MESSAGE_SIZE));

    return std::string(buf, MESSAGE_SIZE);
}

std::string readFromSocket(asio::ip::tcp::socket &sock) {
    const unsigned char MESSAGE_SIZE = 7;
    char buf[MESSAGE_SIZE];
    std::size_t total_bytes_read = 0;

    while (total_bytes_read != MESSAGE_SIZE) {
        total_bytes_read += sock.read_some(
                asio::buffer(buf + total_bytes_read,
                             MESSAGE_SIZE - total_bytes_read));
    }

    return std::string(buf, total_bytes_read);
}

int main() {
    std::string raw_ip_address = "127.0.0.1";
    unsigned short port_num = 3333;

    try {
        asio::ip::tcp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);

        asio::io_service ios;

        asio::ip::tcp::socket sock(ios, ep.protocol());

        sock.connect(ep);

        readFromSocket(sock);
    } catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * The following algorithm describes the steps required to synchronously read data from a TCP socket in a distributed application:
 * (1) In a client application, allocate, open, and connect an active TCP socket. In a server application, obtain a connected
 * active TCP socket by accepting a connection request using an acceptor socket.
 * (2) Allocate the buffer of a sufficient size to fit in the expected message to be read.
 * (3) In a loop, call the socket's read_some() method as many times as it is needed to read the message.
 *
 * The following code sample demonstrates a client application, which operates according to the algorithm:
 */
