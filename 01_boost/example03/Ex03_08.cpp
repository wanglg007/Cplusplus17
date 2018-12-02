//Using boost::find_first
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>

int main() {
    const char *haystack = "Mary had a little lamb";
    const char *needles[] = {"little", "Little", 0};

    for (int i = 0; needles[i] != 0; ++i) {
        auto ret = boost::find_first(haystack, needles[i]);         //从头查找字符串中的子字符串，返回这个子串在原串中的iterator_range迭代器

        if (ret.begin() == ret.end()) {
            std::cout << "String [" << needles[i] << "] not found in" << " string [" << haystack << "\n";
        } else {
            std::cout << "String [" << needles[i] << "] found at " << "offset " << ret.begin() - haystack
                      << " in string [" << haystack << "\n";
        }

        std::cout << "'" << ret << "'" << '\n';
    }
}

