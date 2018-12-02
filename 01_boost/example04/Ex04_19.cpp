//Assigning elements to other containers
#include <string>
#include <map>
#include <list>
#include <deque>
#include <boost/assign.hpp>
#include <iostream>
#include <boost/tuple/tuple.hpp>

using namespace boost::assign;

/**
 * Assigning a list of values to a vector was never as much fun as with Boost.Assign.By
 * overloading the comma operator (operator,) and operator+=, the Boost Assign library
 * provides an easy way to append a list of values to a vector. In order to use the operators,
 * we include boost/assign.hpp (line 3). The using namespace directive makes the operators
 * defined in Boost Assign available in the global scope.Without this, we would not be able
 * to freely use the operators and the expressiveness would be gone. We append three
 * "good morning" greetings in English, French, and Italian to the vector greetings, and
 * then three more in Hebrew, German,and French. The net effect is a vector with six strings.
 * We could have replaced the vector with a deque and this would have still worked. If you
 * wanted an alternate mode of insertion like inserting at the head of a list or deque or
 * inserting into a map, Boost Assign can still work for you. H
 * @return
 */
int main() {
    std::deque<std::string> greets;
    push_front(greets) = "Good night", "Buenas noches",
            "Bounanotte", "Lyla tov", "Gute nacht", "Bonne nuit";

    std::map<std::string, std::string> rockCharacters;
    insert(rockCharacters)
            ("John Barleycorn", "must die")       // Traffic
            ("Eleanor Rigby", "lives in a dream") // Beatles
            ("Arnold Layne", "had a strange hobby")   // Floyd
            ("Angie", "can't say we never tried")    // Stones
            ("Harry", "play the honkytonk"); // Dire Straits

    std::list<boost::tuple<std::string, std::string,
            std::string>> trios;
    push_back(trios)("Athos", "Porthos", "Aramis")
            ("Potter", "Weasley", "Granger")
            ("Tintin", "Snowy", "Haddock")
            ("Geller", "Bing", "Tribbiani")
            ("Jones", "Crenshaw", "Andrews");

    std::cout << "Night greets:\n";
    for (const auto &greet: greets) {
        std::cout << greet << '\n';
    }

    std::cout << "\nPeople:\n";
    for (const auto &character: rockCharacters) {
        std::cout << character.first << ": "
                  << character.second << '\n';
    }

    std::cout << "Trios:\n";
    for (auto &trio: trios) {
        std::cout << boost::get<0>(trio) << ", "
                  << boost::get<1>(trio) << ", "
                  << boost::get<2>(trio) << '\n';
    }
}

