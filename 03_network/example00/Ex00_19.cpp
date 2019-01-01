//
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>

boost::mutex global_stream_lock;

void WorkerThread(boost::shared_ptr<boost::asio::io_service> iosvc, int counter) {
    global_stream_lock.lock();
    std::cout << counter << ".\n";
    global_stream_lock.unlock();

    iosvc->run();

    global_stream_lock.lock();
    std::cout << "End.\n";
    global_stream_lock.unlock();
}

int main(void) {
    boost::shared_ptr<boost::asio::io_service> io_svc(
            new boost::asio::io_service
    );

    boost::shared_ptr<boost::asio::io_service::work> worker(
            new boost::asio::io_service::work(*io_svc)
    );

    std::cout << "Press any key to exit!" << std::endl;

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    std::cin.get();

    io_svc->stop();

    threads.join_all();

    return 0;
}


/**
 * We can see in the preceding screenshot that there is a formatting issue here. Because the std::cout object is a global
 * object, writing to it from different threads at once can cause output formatting issues. To solve this issue, we can
 * use a mutex object that can be found in the boost::mutex object provided by the thread library. Mutex is used to synchronize
 * access to any global data or shared data.
 */