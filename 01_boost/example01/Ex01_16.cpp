//Using compile-time asserts
#include <boost/static_assert.hpp>

template<typename T>
class SmallObjectAllocator {
    BOOST_STATIC_ASSERT(sizeof(T) <= 16);

public:
    SmallObjectAllocator() {}
};

struct Foo {
    char data[32];
};

int main() {
    SmallObjectAllocator<int> intAlloc;
//    SmallObjectAllocator<Foo> fooAlloc; // ERROR: sizeof(Foo) > 16
}

/**
 * (1)assert()函数的作用
 * assert在c/c++中是宏。assert宏定义在头文件<assert.h>中。其原型如下：#define assert(_Expression) (void)( (!!(_Expression)) || (_wassert(_CRT_WIDE(#_Expression), _CRT_WIDE(__FILE__), __LINE__), 0) )
 * assert的作用是先计算表达式_Expression，如果其值为假（即为0），那么它会打印出来assert的内容和__FILE__, __LINE__，然后执行abort()函数使kernel杀掉自己并coredump
 * （是否生成coredump文件，取决于系统配置）；否则，assert()无任何作用。宏assert（）一般用于确认程序的正常操作，其中表达式构造无错时才为真值。完成调试后，不必从源
 * 代码中删除assert()语句，因为宏NDEBUG有定义时，宏assert()的定义为空。
 * 当不想使用assert进行断言时候，可以通过在包含#include <assert.h>的语句之前插入 #define NDEBUG 来禁用assert调用。
 * (2)BOOST_STATIC_ASSERT
 * 对于assert是运行时的断言，它只在运行时断言表达式的值。在编译期间出现的错误时不会提示的。如果需要在编译期间就需要确认某些表达式是否是所期望的那样，比方说结构体的字节对齐
 */
