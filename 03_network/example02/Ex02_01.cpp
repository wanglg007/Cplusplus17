//Using fixed length I/O buffers(Preparing a buffer for an input operation)
#include <boost/asio.hpp>
#include <iostream>
#include <memory>               // For std::unique_ptr<>

using namespace boost;

int main() {
    // We expect to receive a block of data no more than 20 bytes long.
    const size_t BUF_SIZE_BYTES = 20;

    // Step 1. Allocating the buffer.
    std::unique_ptr<char[]> buf(new char[BUF_SIZE_BYTES]);

    // Step 2. Creating buffer representation that satisfies MutableBufferSequence concept requirements.
    asio::mutable_buffers_1 input_buf = asio::buffer(static_cast<void *>(buf.get()), BUF_SIZE_BYTES);

    // Step 3. 'input_buf' is the representation of the buffer 'buf' that can be used in Boost.Asio input operations.

    return 0;
}

/**
 * The following algorithm and corresponding code sample describes how to prepare the buffer that can be used with the
 * Boost.Asios socket's method that performs an input operation such as asio::ip::tcp::socket::receive() or the asio::read()
 * free function:
 * (1) Allocate a buffer. The size of the buffer must be big enough to fit the block of data to be received. Note that this
 * step does not involve any functionalities or data types from Boost.Asio.
 * (2) Represent the buffer using an object that satisfies the MutableBufferSequence concept's requirements.
 * (3) The buffer is ready to be used with Boost.Asio input methods and functions.
 *
 * Let's say we want to receive a block of data from the server. To do this, we first need to prepare a buffer where the
 * data will be stored. This is how we do this in the following code:
 */
