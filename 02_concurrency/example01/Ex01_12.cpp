//使用 std::call_once 作为类成员的延迟初始化(线程安全)
#include <mutex>

struct connection_info {
};

struct data_packet {
};

struct connection_handle {
    void send_data(data_packet const &) {           // 1
    }

    data_packet receive_data() {                     // 3
        return data_packet();
    }
};

struct remote_connection_manager {
    connection_handle open(connection_info const&){
        return connection_handle();
    }
} connection_manager;

class X {
private:
    connection_info connection_details;
    connection_handle connection;
    std::once_flag connection_init_flag;

    void open_connection() {
        connection = connection_manager.open(connection_details);
    }

public:
    X(connection_info const &connection_details_) : connection_details(connection_details_) {}

    void send_data(data_packet const &data) {
        std::call_once(connection_init_flag, &X::open_connection, this);
        connection.send_data(data);
    }

    data_packet receive_data() {
        std::call_once(connection_init_flag, &X::open_connection, this);
        return connection.receive_data();
    }
};

int main() {

}

/**
 * 例子中，第一个调用send_data()①或receive_data()③的线程完成初始化过程。使用成员函数open_connection()去初始化数据，也需要将this指针传进去。和其在在标准库中的函数
 * 一样，其接受可调用对象，比如 std::thread 的构造函数和 std::bind() ，通过向 std::call_once() ②传递一个额外的参数来完成这个操作。
 *
 * 初始化及定义完全在一个线程中发生，并且没有其他线程可在初始化完成前对其进行处理，条件竞争终止于初始化阶段，这样比在之后再去处理好的多。在只需要一个全局实例情况下，
 * 这里提供一个 std::call_once 的替代方案
 *  class my_class;
 *  my_class& get_my_class_instance()
 *  {
 *   static my_class instance;  //线程安全的初始化过程
 *   return instance;
 *  }
 * 多线程可以安全的调用get_my_class_instance()①函数，不用为数据竞争而担心。
 */