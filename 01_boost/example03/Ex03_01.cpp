//Changing a string to uppercase using std::transform
#include <string>
#include <algorithm>
#include <cassert>
#include <cctype>

int main() {
    std::string song = "Green-tinted sixties mind";
    std::transform(song.begin(), song.end(), song.begin(), ::toupper);

    assert(song == "GREEN-TINTED SIXTIES MIND");
}

/**
 * std::transform在指定的范围内应用于给定的操作，并将结果存储在指定的另一个范围内。要使用std::transform函数需要包含<algorithm>头文件。
 */