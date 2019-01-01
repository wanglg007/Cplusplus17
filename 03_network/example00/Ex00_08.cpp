//
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <iostream>

void Print1() {
    for (int i = 0; i < 5; i++) {
        boost::this_thread::sleep_for(boost::chrono::milliseconds{500});
        std::cout << "[Print1] Line: " << i << '\n';
    }
}

void Print2() {
    for (int i = 0; i < 5; i++) {
        boost::this_thread::sleep_for(boost::chrono::milliseconds{500});
        std::cout << "[Print2] Line: " << i << '\n';
    }
}

int main(void) {
    boost::thread_group threads;

    threads.create_thread(Print1);
    threads.create_thread(Print2);

    threads.join_all();
}

/**
 * We can see from the preceding output that the Print1() and Print2() functions are run concurrently.The Print2() function does not
 * need to wait for the Print1() function to finish executing all the instructions that are to be invoked. This is why we call this
 * concurrent programming.
 *
 */