//Changing a character array to uppercase using boost::to_upper
#include <string>
#include <boost/algorithm/string.hpp>
#include <cassert>

int main() {
    char song[17] = "Book of Taliesyn";
    boost::to_upper(song);                      //字母大小写转化
    assert(std::string(song) == "BOOK OF TALIESYN");
}

