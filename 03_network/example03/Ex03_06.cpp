//Implementing a synchronous parallel TCP server
#include <boost/asio.hpp>

#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

using namespace boost;

class Service {
public:
    Service() {}

    void StartHandligClient(std::shared_ptr<asio::ip::tcp::socket> sock) {

        std::thread th(([this, sock]() {
            HandleClient(sock);
        }));

        th.detach();
    }

private:
    void HandleClient(std::shared_ptr<asio::ip::tcp::socket> sock) {
        try {
            asio::streambuf request;
            asio::read_until(*sock.get(), request, '\n');

            // Emulate request processing.
            int i = 0;
            while (i != 1000000)
                i++;

            std::this_thread::sleep_for(
                    std::chrono::milliseconds(500));

            // Sending response.
            std::string response = "Response\n";
            asio::write(*sock.get(), asio::buffer(response));
        }
        catch (system::system_error &e) {
            std::cout << "Error occured! Error code = "
                      << e.code() << ". Message: "
                      << e.what();
        }

        // Clean-up.
        delete this;
    }
};

class Acceptor {
public:
    Acceptor(asio::io_service &ios, unsigned short port_num) :
            m_ios(ios),
            m_acceptor(m_ios,
                       asio::ip::tcp::endpoint(
                               asio::ip::address_v4::any(),
                               port_num)) {
        m_acceptor.listen();
    }

    void Accept() {
        std::shared_ptr<asio::ip::tcp::socket>
                sock(new asio::ip::tcp::socket(m_ios));

        m_acceptor.accept(*sock.get());

        (new Service)->StartHandligClient(sock);
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
    }
    catch (system::system_error &e) {
        std::cout << "Error occured! Error code = "
                  << e.code() << ". Message: "
                  << e.what();
    }

    return 0;
}

/**
 * A synchronous parallel TCP server is a part of a distributed application that satisfies the following criteria:
 * 1> Acts as a server in the client-server communication model
 * 2> Communicates with client applications over TCP protocol
 * 3> Uses I/O and control operations that block the thread of execution until the corresponding operation completes, or an error occurs
 * 4> May handle more than one client simultaneously
 *
 * A typical synchronous parallel TCP server works according to the following algorithm:
 * (1) Allocate an acceptor socket and bind it to a particular TCP port.
 * (2) Run a loop until the server is stopped:
 * (2.1) Wait for the incoming connection request from a client
 * (2.2) Accept the client's connection request
 * (2.3) Spawn a thread of control and in the context of this thread:
 * (2.3.1) Wait for the request message from the client
 * (2.3.2) Read the request message
 * (2.3.3) Process the request
 * (2.3.4) Send a response message to the client
 * (2.3.5) Close the connection with the client and deallocate the socket
 *
 * This recipe demonstrates how to implement a synchronous parallel TCP server application with Boost.Asio.
 */

