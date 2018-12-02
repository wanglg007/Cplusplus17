//Using Boost pointer containers
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>
#include <iostream>
#include <boost/current_function.hpp>

/**
 * In the preceding example, AbstractJob is an abstract base class which defines two private pure virtual functions doStep1 and doStep2 , and
 * a non-virtual public function doJob which calls these two functions. JobA and JobB are two concrete implementations of AbstractJob, which implement the
 * virtual functions doStep1 and doStep2. The override keyword trailing the function signature is a C++11 feature that clarifies that a particular
 * function overrides a virtual function in the base class.
 */
class AbstractJob {
public:
    virtual ~AbstractJob() {}

    void doJob() {
        doStep1();
        doStep2();
    }

private:
    virtual void doStep1() = 0;

    virtual void doStep2() = 0;
};

class JobA : public AbstractJob {
    void doStep1() override {
        std::cout << BOOST_CURRENT_FUNCTION << '\n';
    }

    void doStep2() override {
        std::cout << BOOST_CURRENT_FUNCTION << '\n';
    }
};

class JobB : public AbstractJob {
    void doStep1() override {
        std::cout << BOOST_CURRENT_FUNCTION << '\n';
    }

    void doStep2() override {
        std::cout << BOOST_CURRENT_FUNCTION << '\n';
    }
};

int main() {
    boost::ptr_vector<AbstractJob> basePtrVec;

    basePtrVec.push_back(new JobA);
    basePtrVec.push_back(new JobB);

    AbstractJob &firstJob = basePtrVec.front();
    AbstractJob &lastJob = basePtrVec.back();

    for (auto &job : basePtrVec) {
        job.doJob();
    }
}

