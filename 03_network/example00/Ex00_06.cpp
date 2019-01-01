//
#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <iostream>

int main(void) {
    int guessNumber;
    std::cout << "Select number among 0 to 10: ";
    std::cin >> guessNumber;
    if (guessNumber < 0 || guessNumber > 10) {
        return 1;
    }

    boost::random::mt19937 rng;
    //boost::random::random_device rng;
    boost::random::uniform_int_distribution<> ten(0, 10);
    int randomNumber = ten(rng);

    if (guessNumber == randomNumber) {
        std::cout << "Congratulation, " << guessNumber << " is your lucky number.\n";
    } else {
        std::cout << "Sorry, I'm thinking about number " << randomNumber << "\n";
    }
    return 0;
}

/**
 * As we can see from this example, we always get number 8. This is because we apply Mersenne Twister, a Pseudorandom Number
 * Generator (PRNG), which uses the default seed as a source of randomness so it will generate the same number every time the
 * program is run. And, of course, it is not the program that we expect.
 */