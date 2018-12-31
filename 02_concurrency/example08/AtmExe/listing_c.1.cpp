//简单的消息队列
#include<mutex>
#include<condition_variable>
#include<queue>
#include<memory>


namespace messaging {
    struct message_base {                   // 队列项的基础类
        virtual ~message_base() {}
    };

    template<typename Msg>
    struct wrapped_message :                // 每个消息类型都需要特化
            message_base {
        Msg contents;

        explicit wrapped_message(Msg const &contents_) :
                contents(contents_) {}
    };

    class queue {                           // 我们的队列
        std::mutex m;
        std::condition_variable c;
        std::queue<std::shared_ptr<message_base> > q;
    public:
        template<typename T>
        void push(T const &msg) {
            std::lock_guard<std::mutex> lk(m);
            q.push(std::make_shared<wrapped_message<T> >(msg));
            c.notify_all();
        }

        std::shared_ptr<message_base> wait_and_pop() {
            std::unique_lock<std::mutex> lk(m);
            c.wait(lk, [&] { return !q.empty(); });     // 当队列为空时阻塞
            auto res = q.front();
            q.pop();
            return res;
        }
    };
}
