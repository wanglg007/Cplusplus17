//Generating non-contiguous ranges with Boost.Range adaptors
#include <boost/range.hpp>
#include <boost/range/adaptors.hpp>
#include <string>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <cassert>

int main() {
    std::string str = "funny text";
    auto range = str | boost::adaptors::strided(2);
    boost::to_upper(range);
    assert(str == "FuNnY TeXt");
}

/**
 试用Range Adaptors:
 (1)案例1：
    #include <iostream>
    #include <vector>
    #include <boost/assign.hpp>
    #include <boost/range/adaptor/transformed.hpp>
    #include <boost/spirit/include/karma.hpp>

    using namespace std;
    using namespace boost::adaptors;
    using namespace boost::assign;
    namespace karma = boost::spirit::karma;

    int f(int x) {return x + 1;}

    struct X
    {
        typedef int result_type;
        int operator()(int x) const {return x + 1;}
    };

    int main()
    {
        vector<int> v;
        v += 1,2,3,4,5;

        auto v2 = v | transformed(f);
        cout << karma::format(karma::auto_ % ',' << '/n', v2);      //2,3,4,5,6

        auto v3 = v | transformed(X());
        cout << karma::format(karma::auto_ % ',' << '/n', v3);      //2,3,4,5,6

        return 0;
    }

    案例2：
    #include <iostream>
    #include <vector>
    #include <boost/assign.hpp>
    #include <boost/range/adaptor/transformed.hpp>
    #include <boost/lambda/lambda.hpp>
    #include <boost/spirit/include/phoenix.hpp>
    #include <boost/spirit/include/karma.hpp>
    #include <boost/function_types/result_type.hpp>

    using namespace std;
    using namespace boost::adaptors;
    using namespace boost::assign;
    namespace karma = boost::spirit::karma;

    template<typename F>
    struct result_type_wrapper : boost::function_types::result_type<decltype(&F::operator())>
    {
    };

    template<typename F>
    struct adaptable_wrapper : F
    {
        typedef typename result_type_wrapper<F>::type result_type;
        //typedef typename boost::function_types::result_type<decltype(&F::operator())>::type result_type;

        adaptable_wrapper(F f) : F(f) {}
    };

    template<typename F>
    adaptable_wrapper<F> make_adaptable(F f)
    {
        return adaptable_wrapper<F>(f);
    }

    int main()
    {
        vector<int> v;
        v += 1,2,3,4,5;

        //auto v2 = v | transformed(boost::phoenix::arg_names::_1 + 1);	// error
        //auto v3 = v | transformed(boost::lambda::_1 + 1);				// error
        //auto v4 = v | transformed([](int x){return x + 1;});			// error

        auto v5 = v | transformed(make_adaptable([](int x){return x + 1;}));
        cout << karma::format(karma::auto_ % ',' << '/n', v5);

        return 0;
    }

 */