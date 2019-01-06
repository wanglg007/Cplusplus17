//Implementing a synchronous iterative TCP server
#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service {
public:
    Service() {}

    void HandleClient(asio::ip::tcp::socket &sock) {
        try {
            asio::streambuf request;
            asio::read_until(sock, request, '\n');

            // Emulate request processing.
            int i = 0;
            while (i != 1000000)
                i++;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            // Sending response.
            std::string response = "Response\n";
            asio::write(sock, asio::buffer(response));
        } catch (system::system_error &e) {
            std::cout << "Error occured! Error code = "
                      << e.code() << ". Message: "
                      << e.what();
        }
    }
};

class Acceptor {
public:
    Acceptor(asio::io_service &ios, unsigned short port_num) :
            m_ios(ios),
            m_acceptor(m_ios, asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)) {
        m_acceptor.listen();
    }

    void Accept() {
        asio::ip::tcp::socket sock(m_ios);

        m_acceptor.accept(sock);

        Service svc;
        svc.HandleClient(sock);
    }

private:
    asio::io_service &m_ios;
    asio::ip::tcp::acceptor m_acceptor;
};

class Server {
public:
    Server() : m_stop(false) {}

    void Start(unsigned short port_num) {
        m_thread.reset(new std::thread([this, port_num]() {
            Run(port_num);
        }));
    }

    void Stop() {
        m_stop.store(true);
        m_thread->join();
    }

private:
    void Run(unsigned short port_num) {
        Acceptor acc(m_ios, port_num);

        while (!m_stop.load()) {
            acc.Accept();
        }
    }

    std::unique_ptr<std::thread> m_thread;
    std::atomic<bool> m_stop;
    asio::io_service m_ios;
};

int main() {
    unsigned short port_num = 3333;

    try {
        Server srv;
        srv.Start(port_num);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        srv.Stop();
    } catch (system::system_error &e) {
        std::cout << "Error occured! Error code = "
                  << e.code() << ". Message: "
                  << e.what();
    }

    return 0;
}

/**
 * A synchronous iterative TCP server is a part of a distributed application that satisfies the
following criteria:
ff Acts as a server in the client-server communication model
ff Communicates with client applications over TCP protocol
ff Uses I/O and control operations that block the thread of execution until the
corresponding operation completes, or an error occurs
ff Handles clients in a serial, one-by-one fashion
A typical synchronous iterative TCP server works according to the following algorithm:
1. Allocate an acceptor socket and bind it to a particular TCP port.
2. Run a loop until the server is stopped:
1. Wait for the connection request from a client.
2. Accept the client's connection request when one arrives.
3. Wait for the request message from the client.
4. Read the request message.
5. Process the request.
6. Send the response message to the client.
7. Close the connection with the client and deallocate the socket.
This recipe demonstrates how to implement a synchronous iterative TCP server application
with Boost.Asio.
 */
