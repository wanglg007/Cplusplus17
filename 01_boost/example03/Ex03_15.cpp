//Using boost::replace and boost::erase variants
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <cassert>

int main() {
    std::string input = "Hello, World! Hello folks!";
    boost::replace_first(input, "Hello", "Hola");           //从头找到第一个匹配的字符串，将其替换为给定的另外一个字符串
    assert(input == "Hola, World! Hello folks!");
    boost::erase_first(input, "Hello");                     //从头找到第一个匹配的字符串，将其删除
    assert(input == "Hola, World!  folks!");
}

