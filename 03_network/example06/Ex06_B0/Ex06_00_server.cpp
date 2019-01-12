//Here is a simple synchronous server:using boost::asio
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

#define def_myAssert(x)  if(!x) {cout << "ERROR !" <<  __FILE__ \
                        __LINE__ << endl;\
                        bort();}

typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;

void client_session(socket_ptr sock) {
    while (true) {
        char data[512];
        size_t len = sock->read_some(buffer(data));
        if (len > 0)
            write(*sock, buffer("ok", 2));
    }
}

int main() {
    io_service service;
    ip::tcp::endpoint ep(ip::tcp::v4(), 2001);
    ip::tcp::acceptor acc(service, ep);
    while (true) {
        socket_ptr sock(new ip::tcp::socket(service));
        acc.accept(*sock);
        boost::thread(boost::bind(client_session, sock));
    }
    return 0;
}

/**
 * Again, first your program needs at least one io_service instance. You then specify the port you're listening to, and
 * create an acceptor, one object that accepts client connections.
 * In the following loop, you create a dummy socket and wait for a client to connect.Once a connection has been established,
 * you create a thread that will deal with that connection.
 * In the client_session thread, read a client's request, interpret it, and answer back.
 */
