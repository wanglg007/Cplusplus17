//Using associative pointer containers – ptr_set
#include <boost/ptr_container/ptr_set.hpp>
#include <boost/noncopyable.hpp>
#include <string>
#include <iostream>

class Animal : boost::noncopyable {
public:
    virtual ~Animal() {};

    virtual std::string name() const = 0;
};

class SnowLeopard : public Animal {
public:
    SnowLeopard(const std::string &name) : name_(name) {}

    virtual ~SnowLeopard() { std::cout << "~SnowLeopard\n"; }

    std::string name() const override {
        return name_ + ", the snow leopard";
    }

private:
    std::string name_;
};

class Puma : public Animal {
public:
    Puma(const std::string &name) : name_(name) {}

    virtual ~Puma() { std::cout << "~Puma\n"; }

    virtual std::string name() const {
        return name_ + ", the puma";
    }

private:
    std::string name_;
};

bool operator<(const Animal &left, const Animal &right) {
    return left.name() < right.name();
}

/**
 * This shows the use of std::ptr_set to store polymorphic pointers to dynamicallyallocated objects. The Animal abstract base declares
 * a pure virtual function name.Two two derived classes, SnowLeopard and Puma, (representing two real mammal species) override them. We
 * define a ptr_set of Animal pointers called animals. We create two pumas named Kaju and Juki and a snow leopard named Rongi,inserting
 * them into the set animals (lines 54-56). When we iterate through the list,we get references to the dynamically-allocated objects, not
 * pointers. The operator< compares any two animals and orders them lexically by name.Without this operator, we would not be able to define the ptr_set.
 * @return
 */
int main() {
    boost::ptr_set<Animal> animals;
    animals.insert(new Puma("Kaju"));
    animals.insert(new SnowLeopard("Rongi"));
    animals.insert(new Puma("Juki"));

    for (auto &animal :animals) {
        std::cout << animal.name() << '\n';
    }
}

