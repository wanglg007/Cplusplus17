//
#include <boost/asio.hpp>
#include <iostream>

int main(void) {
    boost::asio::io_service io_svc;
    boost::asio::io_service::work worker(io_svc);

    io_svc.run();

    std::cout << "We will not see this line in console window :(" << std::endl;

    return 0;
}

/**
 * The work class is responsible for telling the io_service object when the work starts and when it has finished. It will
 * make sure that the run() function in the io_service object will not exit during the time the work is underway. Also,
 * it will make sure that the run() function does exit when there is no unfinished work remaining. In our preceding code,
 * the work class informs the io_service object that it has work to do, but we do not define what the work is. Therefore,
 * the program will be blocked infinitely and it will not show the output. The reason it has been blocked is because the
 * run() function is invoked even though we can still terminate the program by pressing Ctrl + C.
 */