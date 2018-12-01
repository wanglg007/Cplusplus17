#include <memory>
#include <fstream>

typedef unsigned char byte;

byte *img_rotate(byte *, size_t size, double angle) { return nullptr; }

byte *rotateImage(std::string imgFile, double angle, size_t &sz) {
    // open the file for reading
    std::ifstream imgStrm(imgFile, std::ios::binary);

    if (imgStrm) {
        imgStrm.seekg(0, std::ios::end);
        sz = imgStrm.tellg();      // determine file size
        imgStrm.seekg(0);

        // allocate buffer and read
        std::unique_ptr<byte[]> img(new byte[sz]);
        // read the image contents
        imgStrm.read(reinterpret_cast<char *>(img.get()), sz);
        // process it
        byte first = img[0];  // access first byte
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
 * 使用:
 * (1)unique_ptr拥有它所指向的对象，在某一时刻只能有一个unique_ptr指向特定的对象。当unique_ptr被销毁时，它所指向的对象也会被销毁。
 * 因此不允许多个unique_ptr指向同一个对象，所以不允许拷贝与赋值。
 *
 * 陷阱:
 * (1)不要与裸指针混用:unique_ptr不允许两个独占指针指向同一个对象，在没有裸指针的情况下只能用release获取内存的地址，同时放弃对对象的所有权，
 * 这样有效避免了多个独占指针同时指向一个对象。而使用裸指针就很容器打破这一点
 * (2)记得使用u.release()的返回值:调用u.release()时是不会释放u所指的内存的，这时返回值就是对这块内存的唯一索引，如果没有使用这个返回值释放
 * 内存或是保存起来，这块内存就泄漏
 *
 */
