//Implementing a synchronous UDP client
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

class SyncUDPClient {
public:
    SyncUDPClient() : m_sock(m_ios) {
        m_sock.open(asio::ip::udp::v4());
    }

    std::string
    emulateLongComputationOp(unsigned int duration_sec, const std::string &raw_ip_address, unsigned short port_num) {
        std::string request = "EMULATE_LONG_COMP_OP " + std::to_string(duration_sec) + "\n";
        asio::ip::udp::endpoint ep(asio::ip::address::from_string(raw_ip_address), port_num);
        sendRequest(ep, request);
        return receiveResponse(ep);
    };

private:
    void sendRequest(const asio::ip::udp::endpoint &ep, const std::string &request) {
        m_sock.send_to(asio::buffer(request), ep);
    }

    std::string receiveResponse(asio::ip::udp::endpoint &ep) {
        char response[6];
        std::size_t bytes_recieved = m_sock.receive_from(asio::buffer(response), ep);

        m_sock.shutdown(asio::ip::udp::socket::shutdown_both);
        return std::string(response, bytes_recieved);
    }

private:
    asio::io_service m_ios;
    asio::ip::udp::socket m_sock;
};

int main() {
    const std::string server1_raw_ip_address = "127.0.0.1";
    const unsigned short server1_port_num = 3333;

    const std::string server2_raw_ip_address = "192.168.1.10";
    const unsigned short server2_port_num = 3334;

    try {
        SyncUDPClient client;

        std::cout << "Sending request to the server #1 ... "
                  << std::endl;

        std::string response =
                client.emulateLongComputationOp(10, server1_raw_ip_address, server1_port_num);

        std::cout << "Response from the serever #1 received: "
                  << response << std::endl;

        std::cout << "Sending request to the server #2... "
                  << std::endl;

        response =
                client.emulateLongComputationOp(10, server2_raw_ip_address, server2_port_num);

        std::cout << "Response from the server #2 received: "
                  << response << std::endl;
    }
    catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * A synchronous UDP client is a part of a distributed application that complies with the following statements:
 * 1> Acts as a client in the client-server communication model
 * 2> Communicates with the server application using UDP protocol
 * 3> Uses I/O and control operations (at least those I/O operations that are related to communication with the server) that
 * block the thread of execution until the corresponding operation completes, or an error occurs
 *
 * A typical synchronous UDP client works according to the following algorithm:
 * (1) Obtain an IP-address and a protocol port number of each server the client application is intended to communicate with.
 * (2) Allocate a UDP socket.
 * (3) Exchange messages with the servers.
 * (4) Deallocate the socket.
 * This recipe demonstrates how to implement a synchronous UDP client application with Boost.Asio.
 */
