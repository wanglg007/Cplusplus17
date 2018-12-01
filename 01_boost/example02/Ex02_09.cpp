#include <boost/intrusive_ptr.hpp>
#include <iostream>

namespace NS {
    class Bar {
    public:
        Bar() : refcount_(0) {}

        ~Bar() { std::cout << "~Bar invoked" << '\n'; }

        friend void intrusive_ptr_add_ref(Bar *);

        friend void intrusive_ptr_release(Bar *);

    private:
        unsigned long refcount_;
    };

    void intrusive_ptr_add_ref(Bar *b) {
        b->refcount_++;
    }

    void intrusive_ptr_release(Bar *b) {
        if (--b->refcount_ == 0) {
            delete b;
        }
    }
} // end NS


int main() {
    boost::intrusive_ptr<NS::Bar> pi(new NS::Bar, true);
    boost::intrusive_ptr<NS::Bar> pi2(pi);
    assert(pi.get() == pi2.get());
    std::cout << "pi: " << pi.get() << '\n'
              << "pi2: " << pi2.get() << '\n';
}

/**
 * intrusive_ptr是一种“侵入式”的引用计数指针，实际并不提供引用计数功能，而是要求被存储的对象自己实现引用计数功能。可以应用于以下两种情形：
 * (1)对内存占用要求非常严格，要求必须与原始指针一样；(2)现存代码已经有了引用计数机制管理的对象。 它提供intrusive_ptr_add_ref和intrusive_ptr_release
 * 函数接口供boost::intrusive_ptr调用。
 *
 */