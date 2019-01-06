//Adding SSL/TLS support to server applications
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <thread>
#include <atomic>
#include <iostream>

using namespace boost;

class Service {
public:
    Service() {}

    void handle_client(
            asio::ssl::stream<asio::ip::tcp::socket> &ssl_stream) {
        try {
            // Blocks until the handshake completes.
            ssl_stream.handshake(
                    asio::ssl::stream_base::server);

            asio::streambuf request;
            asio::read_until(ssl_stream, request, '\n');

            // Emulate request processing.
            int i = 0;
            while (i != 1000000)
                i++;
            std::this_thread::sleep_for(
                    std::chrono::milliseconds(500));

            // Sending response.
            std::string response = "Response\n";
            asio::write(ssl_stream, asio::buffer(response));
        }
        catch (system::system_error &e) {
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
            m_acceptor(m_ios,
                       asio::ip::tcp::endpoint(
                               asio::ip::address_v4::any(),
                               port_num)),
            m_ssl_context(asio::ssl::context::sslv23_server) {
        // Setting up the context.
        m_ssl_context.set_options(
                boost::asio::ssl::context::default_workarounds
                | boost::asio::ssl::context::no_sslv2
                | boost::asio::ssl::context::single_dh_use);

        m_ssl_context.set_password_callback(
                [this](std::size_t max_length,
                       asio::ssl::context::password_purpose purpose)
                        -> std::string { return get_password(max_length, purpose); }
        );

        m_ssl_context.use_certificate_chain_file("server.crt");
        m_ssl_context.use_private_key_file("server.key",
                                           boost::asio::ssl::context::pem);
        m_ssl_context.use_tmp_dh_file("dhparams.pem");

        // Start listening for incoming connection requests.
        m_acceptor.listen();
    }

    void accept() {
        asio::ssl::stream<asio::ip::tcp::socket>
                ssl_stream(m_ios, m_ssl_context);

        m_acceptor.accept(ssl_stream.lowest_layer());

        Service svc;
        svc.handle_client(ssl_stream);
    }

private:
    std::string get_password(std::size_t max_length,
                             asio::ssl::context::password_purpose purpose) const {
        return "pass";
    }

private:
    asio::io_service &m_ios;
    asio::ip::tcp::acceptor m_acceptor;

    asio::ssl::context m_ssl_context;
};

class Server {
public:
    Server() : m_stop(false) {}

    void start(unsigned short port_num) {
        m_thread.reset(new std::thread([this, port_num]() {
            run(port_num);
        }));
    }

    void stop() {
        m_stop.store(true);
        m_thread->join();
    }

private:
    void run(unsigned short port_num) {
        Acceptor acc(m_ios, port_num);

        while (!m_stop.load()) {
            acc.accept();
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
        srv.start(port_num);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        srv.stop();
    } catch (system::system_error &e) {
        std::cout << "Error occured! Error code = "
                  << e.code() << ". Message: "
                  << e.what();
    }

    return 0;
}

/**
 * SSL/TLS protocol support is usually added to the server application when the services it provides assumes transmission of
 * sensitive data such as passwords, credit card numbers,personal data, and so on, by the client to the server. In this case,
 * adding SSL/TLS protocol support to the server allows clients to authenticate the server and establish a secure channel to
 * make sure that the sensitive data is protected while being transmitted.
 * Sometimes, a server application may want to use SSL/TLS protocol to authenticate the client; however, this is rarely the
 * case and usually other methods are used to ensure the authenticity of the client (for example, username and password are
 * specified when logging into a mail server).
 * This recipe demonstrates how to implement a synchronous iterative TCP server application supporting SSL/TLS protocol using
 * the Boost.Asio and OpenSSL libraries. The synchronous iterative TCP server application demonstrated in the recipe named
 * Implementing a synchronous iterative TCP server from Chapter 4, Implementing Server Applications, is taken as a base for
 * this recipe and some code changes and additions are made to it in order to add support for SSL/TLS protocol. The code that
 * differs from that of the base implementation of the synchronous iterative TCP server is highlighted so that the code directly
 * related to SSL/TLS support is better distinguished from the rest of the code.
 */

