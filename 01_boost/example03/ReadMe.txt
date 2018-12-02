####################Boost String####################
####################boost::tokenizer案例：
案例1：如何将char_separator作为strtok()函数的替代物来使用。我们指定了三个字符分隔符，它们不出现在输出单词中。我们没有指定任何保留分隔符，
而且缺省情况是忽略空白单词。
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>
int main()
{
  std::string str = ";;Hello|world||-foo--bar;yow;baz|";
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
  boost::char_separator<char> sep("-;|");
  tokenizer tokens(str, sep);
  for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
    std::cout << "<" << *tok_iter << "> ";
  std::cout << "\n";
  return EXIT_SUCCESS;
}
案例2：用两个被弃分隔符'-'和';'以及一个保留分隔符'|'来进行单词分解。我们还指定了当出现两个连续的分隔符时，要在输出中包含空白单词。
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>
int main()
{
    std::string str = ";;Hello|world||-foo--bar;yow;baz|";
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("-;", "|", boost::keep_empty_tokens);
    tokenizer tokens(str, sep);
    for (tokenizer::iterator tok_iter = tokens.begin();
         tok_iter != tokens.end(); ++tok_iter)
      std::cout << "<" << *tok_iter << "> ";
    std::cout << "\n";
    return EXIT_SUCCESS;
}
示例3：使用char_separator的缺省构造函数，按标点和空格来进行单词分解
#include <iostream>
#include <boost/tokenizer.hpp>
#include <string>
int main()
{
   std::string str = "This is,  a test";
   typedef boost::tokenizer<boost::char_separator<char> > Tok;
   boost::char_separator<char> sep; // 缺省构造
   Tok tok(str, sep);
   for(Tok::iterator tok_iter = tok.begin(); tok_iter != tok.end(); ++tok_iter)
     std::cout << "<" << *tok_iter << "> ";
   std::cout << "\n";
   return EXIT_SUCCESS;
}

####################