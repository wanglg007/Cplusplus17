//(7.9）Using string_view parameters
#include <iostream>
#include <iomanip>
#include <string>
#include <string_view>
#include <vector>

using std::string;
using std::vector;

void find_words(vector<string> &words, std::string_view str, std::string_view separators);

void list_words(const vector<string> &words);

int main() {
    string text;                                               // The string to be searched
    std::cout << "Enter some text terminated by *:\n";
    std::getline(std::cin, text, '*');

    const string separators{" ,;:.\"!?'\n"};                 // Word delimiters
    vector<string> words;                                      // Words found

    find_words(words, text, separators);
    list_words(words);

    //测试string_view
    std::string_view sv = "hello";
    std::cout << sv << std::endl;
}

void find_words(vector<string> &words, std::string_view str, std::string_view separators) {
    size_t start{str.find_first_not_of(separators)};        // First word start index
    size_t end{};                                             // Index for end of a word

    while (start != string::npos)                             // Find the words
    {
        end = str.find_first_of(separators, start + 1);         // Find end of  word
        if (end == string::npos)                                // Found a separator?
            end = str.length();                                   // No, so set to last + 1

        words.push_back(std::string{str.substr(start, end - start)});        // Store the word
        start = str.find_first_not_of(separators, end + 1);     // Find 1st character of next word
    }
}

void list_words(const vector<string> &words) {
    std::cout << "Your string contains the following " << words.size() << " words:\n";
    size_t count{};                                           // Number output
    for (const auto &word : words) {
        std::cout << std::setw(15) << word;
        if (!(++count % 5))
            std::cout << std::endl;
    }
    std::cout << std::endl;
}
/**
 * string_view 是C++17所提供的用于处理只读字符串的轻量对象。这里后缀 view 的意思是只读的视图。
 * (1)通过调用 string_view 构造器可将字符串转换为 string_view 对象。
 *  string 可隐式转换为 string_view。
 * (2)string_view 是只读的轻量对象，它对所指向的字符串没有所有权。
 * (3)string_view通常用于函数参数类型，可用来取代 const char* 和 const string&。
 *   string_view 代替 const string&，可以避免不必要的内存分配。
 * (4)string_view的成员函数即对外接口与 string 相类似，但只包含读取字符串内容的部分。
 *   string_view::substr()的返回值类型是string_view，不产生新的字符串，不会进行内存分配。
 *   string::substr()的返回值类型是string，产生新的字符串，会进行内存分配。
 * (5)string_view字面量的后缀是 sv。（string字面量的后缀是 s）

 *
 */