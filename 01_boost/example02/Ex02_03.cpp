#include <boost/scoped_array.hpp>
#include <fstream>

typedef unsigned char byte;

byte *img_rotate(byte *, size_t size, double angle) { return nullptr; }

byte *rotateImage(const std::string &imgFile, double angle, size_t &sz) {
    // open the file for reading
    std::ifstream imgStrm(imgFile, std::ios::binary);

    if (imgStrm) {
        imgStrm.seekg(0, std::ios::end);
        sz = imgStrm.tellg();            // determine file size
        imgStrm.seekg(0);

        // allocate buffer and read
        boost::scoped_array<byte> img(new byte[sz]);
        // read the image contents
        imgStrm.read(reinterpret_cast<char *>(img.get()), sz);
        byte first = img[0];  // indexed access
        return img_rotate(img.get(), sz, angle);
    }

    sz = 0;
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        size_t size = 0;
        rotateImage(argv[1], 3.14159259 / 2, size);
    }
}

/**
 * 主要特点:同scoped_ptr基本一样，只不过可接受数组的new []，多了下标访问操作，其他类似。
 * (1)构造函数指针必须是 new[] 的结果，而不能是 new 表达式的结果;(2)没有 *， -> 操作符重载，因为 scoped_array 持有的不是一个普通指针;
 * (3)析构函数使用 delete[] 释放资源，而不是 delete;(4)提供 operator[] 操作符重载，可以像普通数组一样用下标访问;(5)没有 begin(), end()
 * 等类似容器迭代器操作函数;
 *
 * 主要用法:scoped_array没有给程序增加额外负担，但是 scoped_array 功能有限，不能动态增长，也没有迭代器支持，不能搭配STL算法，仅有一个
 * 纯粹的“裸”数组接口。在需要动态数组的情况下应该使用 std::vector 。
 *
 */
