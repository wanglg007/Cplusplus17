#include <boost/smart_ptr.hpp>
#include <boost/current_function.hpp>
#include <iostream>
#include <cassert>

class CanBeShared
        : public boost::enable_shared_from_this<CanBeShared> {
public:
    ~CanBeShared() {
        std::cout << BOOST_CURRENT_FUNCTION << '\n';
    }

    boost::shared_ptr<CanBeShared> share() {
        return shared_from_this();
    }
};

typedef boost::shared_ptr<CanBeShared> CanBeSharedPtr;

void doWork(CanBeShared &obj) {
    CanBeSharedPtr sp = obj.share();
    std::cout << "Usage count in doWork " << sp.use_count() << '\n';
    assert(sp.use_count() == 2);
    assert(&obj == sp.get());
}

int main() {
    CanBeSharedPtr cbs = boost::make_shared<CanBeShared>();
    doWork(*cbs.get());
    std::cout << cbs.use_count() << '\n';
    assert(cbs.use_count() == 1);
}

/**
 * std::enable_shared_from_this能让一个对象(假设其名为t，且已被一个std::shared_ptr对象pt管理)安全地生成其他额外的std::shared_ptr实例(假设名为 pt1, pt2, ... )，
 * 它们与pt共享对象t的所有权。
 *
 * 使用场合：当类A被share_ptr管理，且在类A的成员函数里需要把当前类对象作为参数传给其他函数时，就需要传递一个指向自身的share_ptr。
 * （1）为何不直接传递this指针？使用智能指针的初衷就是为了方便资源管理，如果在某些地方使用智能指针，某些地方使用原始指针，很容易破坏智能指针的语义，从而产生各
 * 种错误。（2）可以直接传递share_ptr<this>么？答案是不能，因为这样会造成2个非共享的share_ptr指向同一个对象，未增加引用计数导对象被析构两次。示例如下：
 *  #include <memory>
 *  #include <iostream>
 *   class Bad
 *   {
 *   public:
 *       std::shared_ptr<Bad> getptr() {
 *           return std::shared_ptr<Bad>(this);
 *       }
 *       ~Bad() { std::cout << "Bad::~Bad() called" << std::endl; }
 *   };
 *
 *   int main()
 *   {
 *      // 错误的示例，每个shared_ptr都认为自己是对象仅有的所有者
 *      std::shared_ptr<Bad> bp1(new Bad());
 *      std::shared_ptr<Bad> bp2 = bp1->getptr();
 *      // 打印bp1和bp2的引用计数
 *      std::cout << "bp1.use_count() = " << bp1.use_count() << std::endl;
 *      std::cout << "bp2.use_count() = " << bp2.use_count() << std::endl;
 *  }  // Bad 对象将会被删除两次
 * 正确的实现如下：
 *  #include <memory>
 *  #include <iostream>
 *
 *   struct Good : std::enable_shared_from_this<Good> // 注意：继承
 *   {
 *    public:
 *        std::shared_ptr<Good> getptr() {
 *           return shared_from_this();
 *        }
 *       ~Good() { std::cout << "Good::~Good() called" << std::endl; }
 *  };
 *
 *   int main()
 *   {
 *        // 大括号用于限制作用域，这样智能指针就能在system("pause")之前析构
 *       {
 *          std::shared_ptr<Good> gp1(new Good());
 *          std::shared_ptr<Good> gp2 = gp1->getptr();
 *          // 打印gp1和gp2的引用计数
 *          std::cout << "gp1.use_count() = " << gp1.use_count() << std::endl;
 *          std::cout << "gp2.use_count() = " << gp2.use_count() << std::endl;
 *       }
 *   }
 * 为何会出现这种使用场合：因为在异步调用中，存在一个保活机制，异步函数执行的时间点我们是无法确定的，然而异步函数可能会使用到异步调用之前就存在的变量。
 * 为了保证该变量在异步函数执期间一直有效，我们可以传递一个指向自身的share_ptr给异步函数，这样在异步函数执行期间share_ptr所管理的对象就不会析构，所使用
 * 的变量也会一直有效
 */