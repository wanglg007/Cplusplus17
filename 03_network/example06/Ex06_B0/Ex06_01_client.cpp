//To create a basic asynchronous client, you'll do something similar to the following:
#include "iostream"
#include "iomanip"
#include "cassert"
#include "unistd.h"
#include "boost/asio.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/bind.hpp"

using std::cin;
using std::cout;
using std::endl;
using namespace boost::asio;

#define def_myAssert(x)  if(!x) {cout << "ERROR !" <<  __FILE__ \
                        __LINE__ << endl;\
                        bort();}

void connect_handler(const boost::system::error_code &ec) {
    cout << "ok ....................\n";
}

int main() {
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sock(service);
    sock.async_connect(ep, connect_handler);
    service.run();
    return 0;
}

/**
 * Your program needs at least one io_service instance. You specify where you connect to and create the socket.
 * You then connect asynchronously to the address and port once the connection is complete (its completion handler), that
 * is, connect_handler is called. When connect_handler is called, check for the error code (ec), and if successful, you
 * can write asynchronously to the server.
 *
 * Note that the service.run() loop will run as long as there are asynchronous operations pending. In the preceding example,
 * there's only one such operation, that is, the socket async_connect. After that, service.run() exits.Each asynchronous
 * operation has a completion handler, a function that is called when the operation has completed.
 */



