//Pitfalls of copyable concrete base classes
#include <iostream>
#include <boost/ptr_container/ptr_vector.hpp>

struct ConcreteBase {
    virtual void doWork() {}
};

struct Derived1 : public ConcreteBase {
    Derived1(int n) : data(n) {}

    void doWork() override { std::cout << data << "\n"; }

    int data;
};

struct Derived2 : public ConcreteBase {
    Derived2(int n) : data(n) {}

    void doWork() override { std::cout << data << "\n"; }

    int data;
};

/**
 * The preceding code compiles cleanly but may not do what you expect it to do. In the
 * body of the for-loop, we assign each object of a derived class to a base class instance.
 * The copy constructor of ConcreteBase takes effect, and what we get is a
 * sliced object and incorrect behavior
 *
 * Thus, it is a good idea to prevent copying at the outset by deriving the base class itself from boost::noncopyable
 * @return
 */
int main() {
    boost::ptr_vector<ConcreteBase> vec;
    typedef boost::ptr_vector<ConcreteBase>::iterator iter_t;

    vec.push_back(new Derived1(1));
    vec.push_back(new Derived2(2));

    for (iter_t it = vec.begin(); it != vec.end(); ++it) {
        ConcreteBase obj = *it;                     //没有数据输出
        obj.doWork();
    }
}

