//Changing a section of a char array to uppercase using to_upper
#include <string>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include <iostream>

int main() {
    char song[17] = "Book of Taliesyn";

    //示例1：
    typedef boost::iterator_range<char *> RangeType;
    RangeType rng = boost::make_iterator_range(song + 8, song + 16);
    boost::to_upper(rng);
    assert(std::string(song) == "Book of TALIESYN");

    //示例2：
    typedef std::string::iterator iterator;
    typedef std::string::const_iterator const_iterator;
    typedef boost::iterator_range<iterator> irange;
    typedef boost::iterator_range<const_iterator> cirange;

    std::string str = "hello world";
    const std::string cstr = "const world";
    //基本构建方法
    boost::iterator_range<std::string::iterator> ir(str);
    boost::iterator_range<std::string::const_iterator> cir(str);
    //利用make_iterator_range(几种重载函数)
    irange r = boost::make_iterator_range(str);
    r = boost::make_iterator_range(str.begin(), str.end());
    cirange r2 = boost::make_iterator_range(cstr);
    r2 = boost::make_iterator_range(cstr.begin(), cstr.end());
    r2 = boost::make_iterator_range(str);
    assert(r == str);
    assert(r.size() == 11);
    irange r3 = boost::make_iterator_range(str, 1, -1);
    assert(boost::as_literal("ello worl") == r3);
    irange r4 = boost::make_iterator_range(r3, -1, 1);    // 这个也可以理解成复制构造
    assert(str == r4);
    std::cout << r4 << std::endl;

    irange r5 = boost::make_iterator_range(str.begin(), str.begin() + 5);
    assert(r5 == boost::as_literal("hello"));

    //示例3：
    // 数组
    const int SIZE = 9;
    typedef int array_t[SIZE];
    const array_t ca = {1, 2, 3, 4, 5, 6, 7, 8, 10};

    assert((boost::is_same<boost::range_iterator<array_t>::type, int *>::value));
    assert((boost::is_same<boost::range_value<array_t>::type, int>::value));
    assert((boost::is_same<boost::range_difference<array_t>::type, std::ptrdiff_t>::value));
    assert((boost::is_same<boost::range_size<array_t>::type, std::size_t>::value));
    assert((boost::is_same<boost::range_const_iterator<array_t>::type, const int *>::value));

    assert(boost::begin(ca) == ca);
    assert(boost::end(ca) == ca + boost::size(ca));
    assert(boost::empty(ca) == false);
}

/**
 * boost::range(区间)
 * 一个区间提供了可以访问半开放区间[first,one_past_last)中元素的迭代器，还提供了区间中的元素数量的信息。
 */