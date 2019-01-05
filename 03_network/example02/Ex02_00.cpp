//Using fixed length I/O buffers(Preparing a buffer for an output operation)
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    std::string buf;                // 'buf' is the raw buffer.
    buf = "Hello";                  // Step 1 and 2 in single line.

    // Step 3. Creating buffer representation that satisfies ConstBufferSequence concept requirements.
    asio::const_buffers_1 output_buf = asio::buffer(buf);

    // Step 4. 'output_buf' is the representation of the buffer 'buf' that can be used in Boost.Asio output operations.

    return 0;
}

/**
 * The following algorithm and corresponding code sample describes how to prepare a buffer that can be used with the Boost.Asio
 * socket's method that performs an output operation such as asio::ip::tcp::socket::send() or the asio::write()free function:
 * (1) Allocate a buffer. Note that this step does not involve any functionality or data types from Boost.Asio.
 * (2) Fill the buffer with the data that is to be used as the output.
 * (3) Represent the buffer as an object that satisfies the ConstBufferSequence concept's requirements.
 * (4) The buffer is ready to be used with Boost.Asio output methods and functions.
 *
 * Let's say we want to send a string Hello to the remote application. Before we send the data using Boost.Asio, we need to
 * properly represent the buffer. This is how we do this in the following code:
 */
