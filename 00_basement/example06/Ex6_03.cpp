//(6.3)访问字符串中的某个字符
#include <iostream>
#include <string>
#include <cctype>        // for std::isalpha() and tolower()

int main() {
    std::string text;                              // Stores the input
    std::cout << "Enter a line of text:\n";
    std::getline(std::cin, text);                  // Read a line including spaces

    unsigned vowels{};                            // Count of vowels
    unsigned consonants{};                        // Count of consonants
    for (const auto &ch : text) {
        if (std::isalpha(ch))                        // Check for a letter
        {
            switch (std::tolower(ch))                  // Convert to lowercase
            {
                case 'a':
                case 'e':
                case 'i':
                case 'o':
                case 'u':
                    ++vowels;
                    break;

                default:
                    ++consonants;
                    break;
            }
        }
    }

    std::cout << "Your input contained "
              << vowels << " vowels and "
              << consonants << " consonants."
              << std::endl;
}

