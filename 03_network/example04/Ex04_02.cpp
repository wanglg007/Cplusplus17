//Adding SSL/TLS support to client applications
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <iostream>

using namespace boost;

class SyncSSLClient {
public:
    SyncSSLClient(const std::string &raw_ip_address,
                  unsigned short port_num) :
            m_ep(asio::ip::address::from_string(raw_ip_address),
                 port_num),
            m_ssl_context(asio::ssl::context::sslv3_client),
            m_ssl_stream(m_ios, m_ssl_context) {
        // Set verification mode and designate that
        // we want to perform verification.
        m_ssl_stream.set_verify_mode(asio::ssl::verify_peer);

        // Set verification callback.
        m_ssl_stream.set_verify_callback([this](
                bool preverified,
                asio::ssl::verify_context &context) -> bool {
            return on_peer_verify(preverified, context);
        });
    }

    void connect() {
        // Connect the TCP socket.
        m_ssl_stream.lowest_layer().connect(m_ep);

        // Perform the SSL handshake.
        m_ssl_stream.handshake(asio::ssl::stream_base::client);
    }

    void close() {
        // We ignore any errors that might occur
        // during shutdown as we anyway can't
        // do anything about them.
        boost::system::error_code ec;

        m_ssl_stream.shutdown(ec); // Shutown SSL.

        // Shut down the socket.
        m_ssl_stream.lowest_layer().shutdown(
                boost::asio::ip::tcp::socket::shutdown_both, ec);

        m_ssl_stream.lowest_layer().close(ec);
    }

    std::string emulate_long_computation_op(
            unsigned int duration_sec) {

        std::string request = "EMULATE_LONG_COMP_OP "
                              + std::to_string(duration_sec)
                              + "\n";

        send_request(request);
        return receive_response();
    };

private:
    bool on_peer_verify(bool preverified,
                        asio::ssl::verify_context &context) {
        // Here the certificate should be verified and the
        // verification result should be returned.
        return true;
    }

    void send_request(const std::string &request) {
        asio::write(m_ssl_stream, asio::buffer(request));
    }

    std::string receive_response() {
        asio::streambuf buf;
        asio::read_until(m_ssl_stream, buf, '\n');

        std::string response;
        std::istream input(&buf);
        std::getline(input, response);

        return response;
    }

private:
    asio::io_service m_ios;
    asio::ip::tcp::endpoint m_ep;

    asio::ssl::context m_ssl_context;
    asio::ssl::stream<asio::ip::tcp::socket> m_ssl_stream;
};

int main() {
    const std::string raw_ip_address = "127.0.0.1";
    const unsigned short port_num = 3333;

    try {
        SyncSSLClient client(raw_ip_address, port_num);

        // Sync connect.
        client.connect();

        std::cout << "Sending request to the server... "
                  << std::endl;

        std::string response =
                client.emulate_long_computation_op(10);

        std::cout << "Response received: " << response
                  << std::endl;

        // Close the connection and free resources.
        client.close();
    }
    catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * Client applications usually use SSL/TLS protocol to send sensitive data such as passwords,credit card numbers, personal data.
 * SSL/TLS protocol allows clients to authenticate the server and encrypt the data. The authentication of the server allows the
 * client to make sure that the data will be sent to the expected addressee (and not to a malicious one). Data encryption
 * guarantees that even if the transmitted data is intercepted somewhere on its way to the server, the interceptor will not be
 * able to use it.
 * This recipe demonstrates how to implement a synchronous TCP client application supporting SSL/TLS protocol using the Boost.Asio
 * and OpenSSL libraries. The TCP client application demonstrated in the recipe named Implementing synchronous TCP client,
 * Implementing Client Applications, is taken as a base for this recipe, and some code changes and additions are made to it in
 * order to add support for SSL/TLS protocol. The code that differs from that of the base implementation of the synchronous TCP
 * client is highlighted so that the code directly related to SSL/TLS support is better distinguished from the rest of the code.
 */

