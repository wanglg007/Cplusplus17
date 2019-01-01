//
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include <iostream>

boost::mutex global_stream_lock;

void WorkerThread(boost::shared_ptr<boost::asio::io_service> iosvc, int counter) {
    global_stream_lock.lock();
    std::cout << "Thread " << counter << " Start.\n";
    global_stream_lock.unlock();

    iosvc->run();

    global_stream_lock.lock();
    std::cout << "Thread " << counter << " End.\n";
    global_stream_lock.unlock();
}

void Print(int number) {
    std::cout << "Number: " << number << std::endl;
}

int main(void) {
    boost::shared_ptr<boost::asio::io_service> io_svc(
            new boost::asio::io_service
    );

    boost::shared_ptr<boost::asio::io_service::work> worker(
            new boost::asio::io_service::work(*io_svc)
    );

    boost::asio::io_service::strand strand(*io_svc);

    global_stream_lock.lock();
    std::cout << "The program will exit once all work has finished.\n";
    global_stream_lock.unlock();

    boost::thread_group threads;
    for (int i = 1; i <= 5; i++)
        threads.create_thread(boost::bind(&WorkerThread, io_svc, i));

    boost::this_thread::sleep(boost::posix_time::milliseconds(500));

    strand.post(boost::bind(&Print, 1));
    strand.post(boost::bind(&Print, 2));
    strand.post(boost::bind(&Print, 3));
    strand.post(boost::bind(&Print, 4));
    strand.post(boost::bind(&Print, 5));

    worker.reset();

    threads.join_all();

    return 0;
}

/**
 * Now, let us apply the strand function to synchronize the flow of the program.
 *
 * The order of the work is the same as the sequence of the work in our code.
 */

