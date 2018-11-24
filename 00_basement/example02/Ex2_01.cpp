//(2.1)使用cout将变量的值进行打印
#include <iostream>

int main() {
    //定义integer变量
    int apple_count{15};                            // Number of apples
    int orange_count{5};                            // Number of oranges
    int total_fruit{apple_count + orange_count};    // Total number of fruit

    std::cout << "The value of apple_count is " << apple_count << std::endl;
    std::cout << "The value of orange_count is " << orange_count << std::endl;
    std::cout << "The value of total_fruit is " << total_fruit << std::endl;
}
