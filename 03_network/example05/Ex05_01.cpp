//Preparing a composite buffer for an input operation
#include <boost/asio.hpp>

using namespace boost;

int main() {
    // Step 1. Allocate simple buffers.
    char part1[6];
    char part2[3];
    char part3[7];

    // Step 2. Create an object representing a composite buffer.
    std::vector<asio::mutable_buffer> composite_buffer;

    // Step 3. Add simple buffers to the composite buffer object.
    composite_buffer.push_back(asio::mutable_buffer(part1,
                                                    sizeof(part1)));
    composite_buffer.push_back(asio::mutable_buffer(part2,
                                                    sizeof(part2)));
    composite_buffer.push_back(asio::mutable_buffer(part3,
                                                    sizeof(part3)));

    // Now composite_buffer can be used with Boost.Asio
    // input operation as if it was a simple buffer
    // represented by contiguous block of memory.

    return 0;
}

/**
 * The following is the algorithm and corresponding code sample that describe how to prepare the composite buffer that is
 * to be used with the socket's method that performs an input operation such as asio::ip::tcp::socket::receive() or a free
 * function such as asio::read():
 * (1) Allocate as many memory buffers as required to perform the task at hand. The sum of the sizes of the buffers must
 * be equal to or greater than the size of the expected message to be received in these buffers. Note that this step does
 * not involve any functionalities or data types from Boost.Asio.
 * (2) Create an instance of a class that satisfies the MutableBufferSequence concept's requirements that represents a
 * composite buffer.
 * (3) Add simple buffers to the composite buffer. Each simple buffer should be represented as an instance of the
 * asio::mutable_buffer class.
 * (4) The composite buffer is ready to be used with Boost.Asio input operations.
 *
 * Let's imagine a hypothetical situation, where we want to receive 16 bytes long messages from the server. However, we
 * do not have a buffer that can fit the entire message. Instead, we have three buffers: 6, 3, and 7 bytes long. To create
 * a buffer in which we can receive 16 bytes of data, we can join our three small buffers into a composite one. This is
 * how we do it in the following code:
 */

