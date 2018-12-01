//Pretty printing current function name
#include <boost/current_function.hpp>
#include <iostream>

namespace FoFum {
    class Foo {
    public:
        void bar() {
            std::cout << BOOST_CURRENT_FUNCTION << '\n';
            bar_private(5);
        }

        static void bar_static() {
            std::cout << BOOST_CURRENT_FUNCTION << '\n';
        }

    private:
        float bar_private(int x) const {
            std::cout << BOOST_CURRENT_FUNCTION << '\n';
            return 0.0;
        }
    };
} // end namespace FoFum

namespace {
    template<typename T>
    void baz(const T &x) {
        std::cout << BOOST_CURRENT_FUNCTION << '\n';
    }
} // end unnamed namespace

int main() {
    std::cout << BOOST_CURRENT_FUNCTION << '\n';
    FoFum::Foo f;
    f.bar();
    FoFum::Foo::bar_static();
    baz(f);
}

/**
 * 备注：
 * (1)为了使用BOOST_CURRENT_FUNCTION宏，需要包含<boost/current_function. hpp>
 * (2)只需要在代码中使用BOOST_CURRENT_FUNCTION宏，就可获得包含该宏的外围函数名称，它表现为一个包含完整函数声明的编译期字符串。如果BOOST_CURRENT_FUNCTION宏不
 * 处于任何函数作用域之内，则行为依编译器而不同
 */
