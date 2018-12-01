//Compile-time visitation of variants
#include <boost/variant.hpp>
#include <iostream>
#include <string>

struct SimpleVariantVisitor : public boost::static_visitor<void> {
    void operator()(const std::string &s) const { std::cout << "String: " << s << '\n'; }

    void operator()(long n) const { std::cout << "long: " << n << '\n'; }
};

int main() {
    boost::variant<std::string, long, double> v1;
    v1 = 993.3773;

    //将给定的访问者应用到给定的variant的内容上，该过程在编译期进行类型安全检查，以确认所有类型均被访问者所处理。
    boost::apply_visitor(SimpleVariantVisitor(), v1);
}
/**
 * 备注:
 *  apply_visitor的行为取决于它所操作的参数的数量(即除访问者以外的其它参数)。该函数的行为如下：
 *  接受一个操作数的重载将以给定的 variant 操作数的内容调用给定的访问者的单参函数调用操作符。
 *  接受两个操作数的重载将以给定的 variant 操作数的内容调用给定的访问者的二元函数调用操作符。
 *  只接受一个访问者的重载将返回一个 泛型函数对象，该对象接受一个或两个参数并以这些参数和 visitor 调用 apply_visitor, 其行为和前面两项相似。
 *  返回： 接受操作数的重载将返回将给定访问者应用于给定操作数所得的结果。只接受一个访问者的重载则返回一个函数对象，以延后将访问者应用于任意操作数。
 *  要求： 给定的访问者必须符合 StaticVisitor 概念的要求，并处理给定 variant 的各个有界类型。
 *  抛出： 接受操作数的重载只会在给定的访问者抛出异常时抛出。只接受一个访问者的重载则不会抛出。(但是请注意，返回的 函数对象 有可能在运行时抛出)
 */
