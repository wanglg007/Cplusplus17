//sender类
namespace messaging {
    class sender {
        queue *q;                   // sender是一个队列指针的包装类
    public:
        sender() : q(nullptr) {}   // sender无队列(默认构造函数)

        explicit sender(queue *q_) : q(q_) {}       // 从指向队列的指针进行构造

        template<typename Message>
        void send(Message const &msg) {             // 将发送信息推送给队列
            if (q) {
                q->push(msg);
            }
        }
    };
}

/**
 *
 *
 */