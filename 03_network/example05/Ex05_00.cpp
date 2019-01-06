//Using composite buffers for scatter/gather operations  -- (Preparing a composite buffer for gather output operations)
#include <boost/asio.hpp>

using namespace boost;

int main() {
    // Steps 1 and 2. Create and fill simple buffers.
    const char *part1 = "Hello ";
    const char *part2 = "my ";
    const char *part3 = "friend!";

    // Step 3. Create an object representing a composite buffer.
    std::vector<asio::const_buffer> composite_buffer;

    // Step 4. Add simple buffers to the composite buffer.
    composite_buffer.push_back(asio::const_buffer(part1, 6));
    composite_buffer.push_back(asio::const_buffer(part2, 3));
    composite_buffer.push_back(asio::const_buffer(part3, 7));

    // Step 5. Now composite_buffer can be used with Boost.Asio output operations as if it was a simple buffer represented
    // by contiguous block of memory.

    return 0;
}

/**
 * The following is the algorithm and corresponding code sample that describe how to prepare the composite buffer that is
 * to be used with the socket's method that performs output operations such as asio::ip::tcp::socket::send() or a free
 * function such as asio::write():
 * (1) Allocate as many memory buffers as needed to perform the task at hand. Note that this step does not involve any
 * functionality or data types from Boost.Asio.
 * (2) Fill the buffers with data to be output.
 * (3) Create an instance of a class that satisfies the ConstBufferSequence or MultipleBufferSequence concept's requirements,
 * representing a composite buffer.
 * (4) Add simple buffers to the composite buffer. Each simple buffer should be represented as an instance of the asio::const_buffer
 * or asio::mutable_buffer classes.
 * (5) The composite buffer is ready to be used with Boost.Asio output functions.
 *
 * Let's say we want to send a string Hello my friend! to the remote application, but our message was broken into three
 * parts and each part was stored in a separate buffer. What we can do is represent our three buffers as a composite buffer,
 * and then, use it in the output operation. This is how we will do it in the following code:
 */