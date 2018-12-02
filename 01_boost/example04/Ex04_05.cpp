//Using slist
#include <boost/container/slist.hpp>
#include <iostream>
#include <string>

/**
 * The boost::container::slist container is a singly-linked list abstraction similar to a
 * container template of the same name that was available in the SGI STL implementation
 * but never made it to the standard. The std::list container is a doubly linked list.
 * @return
 */
int main() {
    boost::container::slist<std::string> geologic_eras;

    geologic_eras.push_front("Neogene");
    geologic_eras.push_front("Paleogene");
    geologic_eras.push_front("Cretaceous");
    geologic_eras.push_front("Jurassic");
    geologic_eras.push_front("Triassic");
    geologic_eras.push_front("Permian");
    geologic_eras.push_front("Carboniferous");
    geologic_eras.push_front("Devonian");
    geologic_eras.push_front("Silurian");
    geologic_eras.push_front("Ordovician");
    geologic_eras.push_front("Cambrian");

    for (const auto &str : geologic_eras) {
        std::cout << str << '\n';
    }
}

