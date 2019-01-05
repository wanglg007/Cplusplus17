//Using extensible stream-oriented I/O buffers
#include <boost/asio.hpp>
#include <iostream>

using namespace boost;

int main() {
    asio::streambuf buf;

    std::ostream output(&buf);

    // Writing the message to the stream-based buffer.
    output << "Message1\nMessage2";

    // Now we want to read all data from a streambuf until '\n' delimiter. Instantiate an intput stream which uses our stream buffer.
    std::istream input(&buf);

    // We'll read data into this string.
    std::string message1;

    std::getline(input, message1);

    // Now message1 string contains 'Message1'.

    return 0;
}

/**
 * The main() application entry point function begins with instantiating an object of the asio::streambuf class named buf.
 * Next, the output stream object of the std::ostream class is instantiated. The buf object is used as a stream buffer for
 * the output stream.
 *
 * In the next line, the Message1\nMessage2 sample data string is written to the output stream object, which in turn
 * redirects the data to the buf stream buffer.
 *
 * Usually, in a typical client or server application, the data will be written to the buf stream buffer by the Boost.Asio
 * input function such as asio::read(), which accepts a stream buffer object as an argument and reads data from the socket
 * to that buffer.
 *
 * Now, we want to read the data back from the stream buffer. To do this, we allocate an input stream and pass the buf
 * object as a stream buffer argument to its constructor. After this, we allocate a string object named message1, and then,
 * use the std::getline function to read part of the string currently stored in the buf stream buffer until the delimiter
 * symbol, \n. As a result, the string1 object contains the Message1 string and the buf stream buffer contains the rest
 * of the initial string after the delimiter symbol, that is, Message2.
 */

