//Implementing a synchronous TCP client
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

class SyncTCPClient {
public:
    SyncTCPClient(const std::string &raw_ip_address, unsigned short port_num) :
            m_ep(asio::ip::address::from_string(raw_ip_address), port_num),
            m_sock(m_ios) {
        m_sock.open(m_ep.protocol());
    }

    void connect() {
        m_sock.connect(m_ep);
    }

    void close() {
        m_sock.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        m_sock.close();
    }

    std::string emulateLongComputationOp(unsigned int duration_sec) {
        std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
        sendRequest(request);
        return receiveResponse();
    };

private:
    void sendRequest(const std::string &request) {
        asio::write(m_sock, asio::buffer(request));
    }

    std::string receiveResponse() {
        asio::streambuf buf;
        asio::read_until(m_sock, buf, '\n');

        std::istream input(&buf);

        std::string response;
        std::getline(input, response);

        return response;
    }

private:
    asio::io_service m_ios;

    asio::ip::tcp::endpoint m_ep;
    asio::ip::tcp::socket m_sock;
};

int main() {
    const std::string raw_ip_address = "127.0.0.1";
    const unsigned short port_num = 3333;

    try {
        SyncTCPClient client(raw_ip_address, port_num);

        // Sync connect.
        client.connect();

        std::cout << "Sending request to the server... " << std::endl;

        std::string response = client.emulateLongComputationOp(10);

        std::cout << "Response received: " << response
                  << std::endl;

        // Close the connection and free resources.
        client.close();
    } catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * A synchronous TCP client is a part of a distributed application that complies with the following statements:
 * 1> Acts as a client in the client-server communication model
 * 2> Communicates with the server application using a TCP protocol
 * 3> Uses I/O and control operations (at least those I/O operations that are related to communication with a server) that
 *  block the thread of execution until the corresponding operation completes, or an error occurs
 *
 * A typical synchronous TCP client works according to the following algorithm:
 * (1) Obtain the IP-address and the protocol port number of the server application.
 * (2) Allocate an active socket.
 * (3) Establish a connection with the server application.
 * (4) Exchange messages with the server.
 * (5) Shut down the connection.
 * (6) Deallocate the socket.
 * This recipe demonstrates how to implement a synchronous TCP client application with Boost.Asio.
 */
