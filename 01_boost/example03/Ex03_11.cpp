//Using boost::iter_find with boost::token_finder
#include <boost/algorithm/string.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>

struct MinLen {
    bool operator()(const std::string &s) const { return s.size() > 3; }
};

int main() {
    std::string str = "The application tried to read from an "
                      "invalid address at 0xbeeffed";

    std::vector<std::string> v;
    auto ret = boost::iter_find(v, str, boost::token_finder(boost::is_xdigit(), boost::token_compress_on));

    std::ostream_iterator<std::string> osit(std::cout, ", ");
    std::copy_if(v.begin(), v.end(), osit, MinLen());
}

/**
 * 备注：copy_if()算法可以从源序列复制使谓词返回true的元素，所以可以把它看作一个过滤器。前两个参数定义源序列的输入迭代器，第三个参数是指向目的序列的第一个位置
 * 的输出迭代器，第四个参数是一个谓词。会返回一个输出迭代器，它指向最后一个被复制元素的下一个位置。
 *
 * 案例：
 *  std::vector<string> names {"A1", "Beth", "Carol", "Dan", "Eve","Fred", "George", "Harry", "Iain", "Joe"};
 *  std::unordered_set<string> more_names {"Jean", "John"};
 *  size_t max_length{4};
 *  std::copy_if(std::begin(names), std::end(names), std::inserter(more_names, std::begin(more_names)), [max_length](const string& s) { return s.length() <= max_length;});
 *  因为作为第4个参数的lambda表达式所添加的条件，这里的copy_if()操作只会复制 names 中的 4 个字符串或更少。目的容器是一个 unordered_set 容器 more_names，
 *  它已经包含两个含有 4 个字符的名称。
 *
 *  copy_if()的目的容器也可以是一个流迭代器，示例如下：
 *  std::vector<string> names { "Al", "Beth", "Carol", "Dan", "Eve","Fred", "George", "Harry", "Iain", "Joe"};
 *  size_t max_length{4};
 *  std::copy_if(std::begin(names), std::end(names), std::ostream iterator< string> {std::cout," "}, [max_length](const string& s) { return s.length() > max_length; });
 *  std::cout << std::endl;
 *  这里会将 names 容器中包含的含有 4 个以上字符的名称写到标准输出流中。
 *
 *  输入流迭代器可以作为 copy_if() 算法的源，也可以将它用在其他需要输入迭代器的算法上，示例如下：
 *  std::unordered_set<string> names;
 *  size_t max_length {4};
 *  std::cout << "Enter names of less than 5 letters. Enter Ctrl+Z on a separate line to end:\n";
 *  std::copy_if(std::istream_iterator<string>{std::cin},std:: istream iterator<string>{}, std::inserter(names, std::begin (names)),[max_length](const string& s) { return s.length() <= max_length; });
 *  std::copy(std::begin(names), std::end(names), std::ostream_iterator <string>{std::cout," "});
 *  std::cout << std::endl;
 *  容器 names 最初是一个空的 unordered_set。只有当从标准输入流读取的姓名的长度小于或等于 4 个字符时，copy_if() 算法才会复制它们。
 */
