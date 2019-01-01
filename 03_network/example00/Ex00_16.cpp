//
#include <boost/bind.hpp>
#include <iostream>

void cubevolume(float f) {
    std::cout << "Volume of the cube is " << f * f * f << std::endl;
}

int main(void) {
    boost::bind(&cubevolume, 4.23f)();
    return 0;
}

/**
 * Now, let's use the function that has arguments to pass. We will use boost::bind for this purpose in the following code:
 */

