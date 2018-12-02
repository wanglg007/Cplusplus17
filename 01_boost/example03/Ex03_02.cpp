//Changing a string to uppercase using boost::to_upper
#include <string>
#include <boost/algorithm/string.hpp>
#include <cassert>

int main() {
    std::string song = "Green-tinted sixties mind";
    boost::to_upper(song);                          //字母大小写转化
    assert(song == "GREEN-TINTED SIXTIES MIND");
}