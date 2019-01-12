//The following code is of a basic asynchronous server:
#include "iostream"
#include "iomanip"
#include "cassert"
#include "unistd.h"
#include "boost/asio.hpp"
#include "boost/smart_ptr.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"

using std::cin;
using std::cout;
using std::endl;
using namespace boost::asio;

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
io_service service;
ip::tcp::endpoint ep(ip::tcp::v4(), 2001); // listen on 2001
ip::tcp::acceptor acc(service, ep);

void start_accept(socket_ptr sock) {
    acc.async_accept(*sock, boost::bind(handle_accept, sock, _1));
}

void handle_accept(socket_ptr sock, const boost::system::error_code &err) {
    if (err) return;
    // at this point, you can read/write to the socket
    socket_ptr sock(new ip::tcp::socket(service));
    start_accept(sock);
}

int main() {
    socket_ptr sock(new ip::tcp::socket(service));
    start_accept(sock);
    service.run();
}

/**
 * In the preceding code snippet, first, you create an io_service instance. You then specify what port you're listening
 * to. Then, you create the acc acceptor, an object to accept client connections and also, create a dummy socket, and
 * asynchronously wait for a client to connect.
 */