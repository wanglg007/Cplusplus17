//Using boost::tokenizer with boost::escaped_list_separator
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>

int main() {
    std::string input = "Amit Gupta,70,\"\\\"Nandanvan\\\", "
                        "Ghole Road, Pune, India\"";

    typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
    tokenizer mytokenizer(input);

    for (auto &tok: mytokenizer) {
        std::cout << tok << '\n';
    }
}
/**
 * boost::tokenizer提供了一种灵活、易用的方式来将一个字符串分解为一个字符串列表。类的声明为：
 * template <
 *       class TokenizerFunc = char_delimiters_separator<char>,
 *       class Iterator = std::string::const_iterator,
 *       class Type = std::string
 * >
 * class tokenizer
 * 它的核心包括:TokenizerFunc：决定了字符串分割方式;Iterator：确定分割后的字符串的迭代类型;Type：需要分割的字符串类型;
 *
 * boost::tokenizer的构造函数有以下两个：
 * tokenizer(Iterator first, Iterator last,const TokenizerFunc& f = TokenizerFunc())
 * template<class Container> tokenizer(const Container& c,const TokenizerFunc& f = TokenizerFunc())
 *
 * tokenizer中使用的TokenFunc在boost标准中有其他3个实现，它们分别有不同的分割规则：
 * (1)char_seperator ：根据分隔符进行分割，可以包含多个分割字符;(2)escaped_list_seperator：用于对csv文件进行分割;
 * (3)offset_seperator：根据偏移量进行分割;
 *
 */

