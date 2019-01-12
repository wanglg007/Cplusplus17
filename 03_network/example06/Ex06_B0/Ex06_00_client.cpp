//Here's a basic example of a synchronous client:
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

int main() {
    io_service service;
    ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 2001);
    ip::tcp::socket sock(service);
    sock.connect(ep);
    return 0;
}

/**
 * First, your program needs at least an io_service instance. Boost.Asio uses io_service to talk to the operating system's
 * input/output services. Usually one instance of an io_service will be enough. Next, create the address and port you want
 * to connect to. Create the socket. Connect the socket to your address and port
 */
