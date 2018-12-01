####################Basic ideas####################
####################
(1)boost::swap
在STL已提供std::swap时为什么还有必要使用boost::swap？std::swap的行为等效于：
template <class T> void swap ( T& a, T& b ) {
  T c(a);
  a=b;
  b=c;
}
对于存储大量数据的类，此方法可能不是交换数据的最有效方法，因为swap涉及到一个copy construction和两次赋值。另外，对于出于设计原因拥有private构造函数而
没有复制构造函数的类，所以这种交换风格不适用。以下是boost::swap提供的功能：(A)可以交换T类型的数组，而std::swap不能；(B)boost::swap可调用具有签名swap(T&, T&)的函数，
只要存在相同的签名，且不存在默认的copy constructor及两个赋值选项；(C)boost::swap可调用std::swap 的一个特殊化模板；(D)如果上面第二和第三个选项都是有效选项，T 必须是
可构造和可赋值的副本。
(1.1)用于交换两个数组的 boost::swap案例：
#include <boost/swap.hpp>
#include <boost/foreach.hpp>
#include <iostream>
using namespace std;
int main() {
  int a[] = {10, 20, 30, 40};
  int b[] = {4, 3, 2, 1};

  boost::swap(a, b);    // using std::swap here won't work
  BOOST_FOREACH(int t, a) { cout << t << endl; }
  BOOST_FOREACH(int t, a) { cout << t << endl; }
}
(1.2)使用boost::swap实现自定义交换案例:
#include <boost/swap.hpp>
#include <iostream>
using namespace std;

typedef struct T {
  int m_data;
  T(int data) : m_data(data) { }
} T;

void swap(T& a, T& b) // custom swap routine that boost ::swap calls
{
  cout << "In custom swap" << endl;
  a.m_data ^= b.m_data;
  b.m_data ^= a.m_data;
  a.m_data ^= b.m_data;
}

int main() {
  T a(30), b(10);
  boost::swap(a, b);
  cout << a.m_data << endl;
  cout << b.m_data << endl;
}
(1.3)使用std::swap的模板特殊化版本
#include <boost/swap.hpp>
#include <iostream>
using namespace std;

typedef struct T {
  int m_data;
  T(int data) : m_data(data) { }
} T;

namespace std {
template<
void swap<T> (T& a, T& b)
{
  cout << "In template-specialized swap" << endl;
  a.m_data ^= b.m_data;
  b.m_data ^= a.m_data;
  a.m_data ^= b.m_data;
}
}

int main() {
  T a(30), b(10);
  boost::swap(a, b);
  cout << a.m_data << endl;
  cout << b.m_data << endl;
}
####################
(2)