//Using Boost Any
#include <boost/any.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <cassert>

using boost::any_cast;

struct MyValue {
    MyValue(int n) : value(n) {}

    int get() const { return value; }

    int value;
};

int main() {
    boost::any v1, v2, v3, v4;

    assert(v1.empty());
    const char *hello = "Hello";
    v1 = hello;
    v2 = 42;
    v3 = std::string("Hola");
    MyValue m1(10);
    v4 = m1;

    try {
        std::cout << any_cast<const char *>(v1) << '\n';
        std::cout << any_cast<int>(v2) << '\n';
        std::cout << any_cast<std::string>(v3) << '\n';
        auto x = any_cast<MyValue>(v4);
        std::cout << x.get() << '\n';
    } catch (std::exception &e) {
        std::cout << e.what() << '\n';
    }
}

/*
 * 备注：
 * boost::any是一个能保存任意类型值的类，有点像variant类型，不过variant由于采用了一个巨大的union，效率非常低。而boost利用模板，保存的时候并不改变值的类型，
 * 只是在需要的时候才提供方法让用户进行类型判断及取值。
 * 优点:
 * 因为多态只有在使用指针或引用的情况下才能显现，所以std容器中只能存放指针或引用（但实际上只能存放指针，无法存放引用）。如：std::list<BaseClass*> mylist;
 * (1)这样就要对指针所指向内容的生存周期操心(可能需要程序员适时删除申请的内存；但是由于存放指针，插入/删除的效率高)，而使用boost::any就可能避免这种情况，因为
 * 可以存放类型本身（当然存放指针也可以）。这是boost::any的优点之一。(2)另一个优点是可以存放任何类型。而前面提到的mylist只能存放BaseClass类指针以及其继承类的指针。
 * 缺点:
 * 由于boost::any可以存放任何类型，自然它用不了多态特性，没有统一的接口，所以在获取容器中的元素时需要实现判别元素的真正类型，这增加了程序员的负担。
 */
