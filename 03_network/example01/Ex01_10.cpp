//Accepting connections
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    // The size of the queue containing the pending connection requests.
    const int BACKLOG_SIZE = 30;

    // Step 1. Here we assume that the server application has already obtained the protocol port number.
    unsigned short port_num = 3333;

    // Step 2. Creating a server endpoint.
    asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port_num);

    asio::io_service ios;

    try {
        // Step 3. Instantiating and opening an acceptor socket.
        asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

        // Step 4. Binding the acceptor socket to the server endpint.
        acceptor.bind(ep);

        // Step 5. Starting to listen to incoming connection requests.
        acceptor.listen(BACKLOG_SIZE);

        // Step 6. Creating an active socket.
        asio::ip::tcp::socket sock(ios);

        // Step 7. Processing the next connection request and connecting the active socket to the client.
        acceptor.accept(sock);

        // At this point 'sock' socket is connected to the client application and can be used to send data to or receive data from it.
    } catch (system::system_error &e) {
        std::cout << "Error occured! Error code = " << e.code()
                  << ". Message: " << e.what();

        return e.code().value();
    }

    return 0;
}

/**
 * The following algorithm describes how to set up an acceptor socket so that it starts listening for incoming connections
 * and then how to use it to synchronously process the pending connection request. The algorithm assumes that only one
 * incoming connection will be processed in synchronous mode:
 * (1) Obtain the port number on which the server will receive incoming connection requests.
 * (2) Create a server endpoint.
 * (3) Instantiate and open an acceptor socket.
 * (4) Bind the acceptor socket to the server endpoint created in step 2.
 * (5) Call the acceptor socket's listen() method to make it start listening for incoming connection requests on the endpoint.
 * (6) Instantiate an active socket object.
 * (7) When ready to process a connection request, call the acceptor socket's accept() method passing an active socket object
 * created in step 6 as an argument.
 * (8) If the call succeeds, the active socket is connected to the client application and is ready to be used for communication with it.
 */


