//(2.3)查找数据类型的最大值和最小值(Finding  maximum and minimum values for data types)
#include <limits>
#include <iostream>

int main()  
{
  std::cout << "The range for type short is from "
            << std::numeric_limits<short>::min() << " to "
            << std::numeric_limits<short>::max()  << std::endl;
  std::cout << "The range for type int is from "
            << std::numeric_limits<int>::min() << " to "
            << std::numeric_limits<int>::max() << std::endl;
  std::cout << "The range for type long is from "
            << std::numeric_limits<long>::min()<< " to "
            << std::numeric_limits<long>::max() << std::endl;
  std::cout << "The range for type float is from "
            << std::numeric_limits<float>::min() << " to "
            << std::numeric_limits<float>::max() << std::endl;
  std::cout << "The positive range for type double is from "
            << std::numeric_limits<double>::min() << " to "
            << std::numeric_limits<double>::max() << std::endl;
  std::cout << "The positive range for type long double is from "
            << std::numeric_limits<long double>::min() << " to "
            << std::numeric_limits<long double>::max() << std::endl;

  //Finding other properties of fundamental types
  float positive_infinity = std::numeric_limits<float>::infinity();
  double negative_infinity = -std::numeric_limits<double>::infinity();
  long double not_a_number = std::numeric_limits<long double>::quiet_NaN();
  std::cout << positive_infinity << "->"
            << negative_infinity << "->"
            << not_a_number;
}