//
#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

class PasswordGenerator {
public:
    std::string Generate(int);
};

std::string PasswordGenerator::Generate(int passwordLength) {
    int randomNumber;
    std::string password;
    std::srand(std::time(0));
    for (int i = 0; i < passwordLength; i++) {
        randomNumber = std::rand() % 94 + 33;
        password += (char) randomNumber;
    }
    return password;
}

int main(void) {
    int passLen;
    std::cout << "Define password length: ";
    std::cin >> passLen;
    PasswordGenerator pg;
    std::string password = pg.Generate(passLen);
    std::cout << "Your password: " << password << "\n";
    return 0;
}