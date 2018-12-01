//Using lexical_cast
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <exception>
#include <iostream>

int main() {
    std::string str = "1234";

    try {
        int n = boost::lexical_cast<int>(str);
        assert(n == 1234);
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }
}

/**
 * 备注:lexical_cast可以用统一的方式来做基本类型之间的转换，比如字符串到数字，数字到字符串，bool和字符串及数字之间的相互转换。
 *
 * 基本方法:
 * (1)lexical_cast在转换字符串时，字符串中只能包含数字和小数点，不能出现除e/E以外的英文字符或者其他非数字字符;
 *  using boost::lexical_cast;
 *  int a = lexical_cast<int>("123");					// string -> int
 *  long b = lexical_cast<long>("2015");				// string -> long
 *  double c = lexical_cast<double>("123.12");			// string -> double
 *  float pai = lexical_cast<float>("3.14159");			// string -> float
 * (2)数字转换成字符串时不支持高级格式控制
 *  string str = lexical_cast<string>(123);				// int -> string
 *  cout << lexical_cast<string>(1.234) << endl;		// float -> string
 *  cout << lexical_cast<string>(0x11) << endl;			// 16进制 -> string
 *  bool bl = lexical_cast<bool>("1");					// string -> bool, only support 1 & 0
 *
 * 异常操作:
 * 当 lexical_cast 无法执行转换时会抛出异常 bad_lexical_cast ，它是 std::bad_cast 的派生类。可以使用 try/catch 块来保护代码。
 * try{
 *       cout << lexical_cast<int>("0x100");
 *       cout << lexical_cast<bool>("false");
 *  }catch (bad_lexical_cast& e){
 *       cout << "error: " << e.what() << endl;
 *  }
 *
 *  转换对象要求:
 * (1)exical_cast 对转换对象有如下要求：
 *  转换起点对象是可流输出的，即定义了 operator«
 *  转换终点对象是可流输入的，即定义了 operator»
 *  转换终点对象必须是可缺省构造和可拷贝构造的
 *  C++中内建类型（int，double等）和std::string 都是符合三个条件的。
 *
 *  自定义类:
 *  如果想要将 lexical_cast 用于自定义类，实现 java 中 Object.toString() 的用法，只需要满足 lexical_cast 的要求，实现流输出操作符 operator« 即可。
 *  class demo_class{
 *      friend ostream& operator<<(ostream& os, const demo_class& x){
 *           os << "class content" << endl;
 *           return os;
 *      }
 *   };
 *   int main() {
 *       demo_class demo = demo_class();
 *       cout << lexical_cast<string>(demo) << endl;
 *  }
 */
