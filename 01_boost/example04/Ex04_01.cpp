//(Pushing String objects onto Boost vectors)Movable but not copyable String
#include <boost/move/move.hpp>
#include <boost/swap.hpp>
#include <cstring>

#include <boost/container/vector.hpp>
#include <boost/foreach.hpp>
#include <iostream>
#include <cassert>

/**
 * Consider the following class for encapsulating char strings, which is movable
 * but not copyable. We use the Boost move emulation macros to define its move
 * semantics. In a C++11 environment, this code translates to C++11 move syntax,
 * while on C++03, it emulates the move semantics:
 */
class String {
private:
BOOST_MOVABLE_BUT_NOT_COPYABLE(String)

public:
    String(const char *s = nullptr) : str(nullptr), sz(0) {
        str = heapcpy(s, sz);
    }

    ~String() {
        delete[] str;
        sz = 0;
    }

    String(BOOST_RV_REF(String)that) : str(nullptr), sz(0) {
        swap(that);
    }

    String &operator=(BOOST_RV_REF(String)rhs) {
        String tmp(boost::move(rhs));

        return *this;
    }

    void swap(String &rhs) {
        boost::swap(this->sz, rhs.sz);
        boost::swap(this->str, rhs.str);
    }

    const char *get() const {
        return str;
    }

private:
    char *str;
    size_t sz;

    static char *heapcpy(const char *str, size_t &sz) {
        char *ret = nullptr;

        if (str) {
            sz = std::strlen(str) + 1;
            ret = new char[sz];
            std::strncpy(ret, str, sz);
        }

        return ret;
    }
};

/**
 * Here is some code that moves String instances into a boost::container::vector, which is the Boost counterpart of std::vector:
 * @return
 */
int main() {
    boost::container::vector<String> strVec;
    String world("world");
    // Move temporary (rvalue)
    strVec.push_back(String("Hello"));
    // Error, copy semantics needed
    //strVec.push_back(world);
    // Explicit move
    strVec.push_back(boost::move(world));
    // world nulled after move
    assert(world.get() == nullptr);
    // in-place construction
    strVec.emplace_back("Hujambo Dunia!"); // Swahili

    BOOST_FOREACH(String &str, strVec) {
                    std::cout << str.get() << '\n';
                }
}
