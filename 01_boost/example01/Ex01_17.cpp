//Using compile-time asserts
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

template<typename T, typename U>
T bitwise_or(const T &left, const U &right) {
    BOOST_STATIC_ASSERT(boost::is_pod<T>::value &&
                                boost::is_pod<U>::value);
    BOOST_STATIC_ASSERT(sizeof(T) >= sizeof(U));

    T result = left;
    const unsigned char *right_array =
            reinterpret_cast<const unsigned char *>(&right);
    unsigned char *left_array =
            reinterpret_cast<unsigned char *>(&result);

    for (size_t i = 0; i < sizeof(U); ++i) {
        left_array[i] |= right_array[i];
    }

    return result;
}

struct X {
    int x;
    int y;
};

struct Y {
    char a;
    char b;
    short s;
};

int main() {
    X x1{64, 96};
    Y y1{'a', 'b', 20};

    X x2 = bitwise_or(x1, y1);
}

/**
 * POD数据类型:
 *      POD全称Plain Old Data。通俗的讲，一个类或结构体通过二进制拷贝后还能保持其数据不变，那么它就是一个POD类型。
 * 平凡的定义:
 *      (1)有平凡的构造函数;(2)有平凡的拷贝构造函数;(3)有平凡的移动构造函数;(4)有平凡的拷贝赋值运算符;(5)有平凡的移动赋值运算符;
 *      (6)有平凡的析构函数;(7)不能包含虚函数;(8)不能包含虚基类;
 * 示例代码如下：
 *  class A { A(){} };
    class B { B(B&){} };
    class C { C(C&&){} };
    class D { D operator=(D&){} };
    class E { E operator=(E&&){} };
    class F { ~F(){} };
    class G { virtual void foo() = 0; };
    class H : G {};
    class I {};

    int main(int argc, _TCHAR* argv[])
    {
        std::cout << std::is_trivial<A>::value << std::endl;  // 有不平凡的构造函数
        std::cout << std::is_trivial<B>::value << std::endl;  // 有不平凡的拷贝构造函数
        std::cout << std::is_trivial<C>::value << std::endl;  // 有不平凡的拷贝赋值运算符
        std::cout << std::is_trivial<D>::value << std::endl;  // 有不平凡的拷贝赋值运算符
        std::cout << std::is_trivial<E>::value << std::endl;  // 有不平凡的移动赋值运算符
        std::cout << std::is_trivial<F>::value << std::endl;  // 有不平凡的析构函数
        std::cout << std::is_trivial<G>::value << std::endl;  // 有虚函数
        std::cout << std::is_trivial<H>::value << std::endl;  // 有虚基类

        std::cout << std::is_trivial<I>::value << std::endl;  // 平凡的类

        return 0;
    }
 */
