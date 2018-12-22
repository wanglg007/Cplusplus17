//不用锁实现push()
#include <atomic>

template<typename T>
class lock_free_stack {
private:
    struct node {
        T data;
        node *next;

        node(T const &data_) : data(data_) {}       // 1
    };

    std::atomic<node *> head;
public:
    void push(T const &data) {
        node *const new_node = new node(data);      // 2
        new_node->next = head.load();                // 3
        while (!head.compare_exchange_weak(new_node->next, new_node));  // 4
    }
};

int main() {}

/**
 * 上面代码匹配三个步骤：创建一个新节点②，设置新节点的next指针指向当前head③，并设置head指针指向新节点④。node结构用其自身的构造函数来进行数据
 * 填充①，必须保证节点在构造完成后随时能被弹出。之后需要使用compare_exchange_weak()来保证在被存储到new_node->next的head指针和之前的一样③。代码
 * 的亮点是使用“比较/交换”操作：当其返回false时，因为比较失败(例如，head被其他线程锁修改)，new_node->next作为操作的第一个参数，将会更新head。
 * 循环中不需要每次都重新加载head指针，因为编译器会帮你完成这件事。同样，因为循环可能直接就失败了，所以这里使用compare_exchange_weak要好于使用
 * compare_exchange_strong。
 *
 * 这里唯一一个能抛出异常的地方就构造新node的时候①，不过其会自行处理，且链表中的内容没有被修改，所以这里是安全的。因为在构建数据的时候，是将其作
 * 为node的一部分作为存储的，并且使用compare_exchange_weak()来更新head指针，所以这里没有恶性的条件竞争。“比较/交换”成功时，节点已经准备就绪，且
 * 随时可以提取。因为这里没有锁，所以就不存在死锁的情况，这里的push()函数实现的很成功。
 */