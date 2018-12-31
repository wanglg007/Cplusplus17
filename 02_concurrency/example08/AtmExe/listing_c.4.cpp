// dispatcher类
namespace messaging {
    class close_queue {             // 用于关闭队列的消息
    };

    class dispatcher {
        queue *q;
        bool chained;

        dispatcher(dispatcher const &) = delete;                // dispatcher实例不能被拷

        dispatcher &operator=(dispatcher const &) = delete;

        template<
                typename Dispatcher,
                typename Msg,
                typename Func>
        friend
        class TemplateDispatcher;                               // 允许TemplateDispatcher实例访问内部成员

        void wait_and_dispatch() {
            for (;;) {                                          // 1 循环，等待调度消息
                auto msg = q->wait_and_pop();
                dispatch(msg);
            }
        }

        bool dispatch(std::shared_ptr <message_base> const &msg) {      // 2 dispatch()会检查close_queue消息，然后抛出
            if (dynamic_cast<wrapped_message <close_queue> *>(msg.get())) {
                throw close_queue();
            }
            return false;
        }

    public:
        dispatcher(dispatcher &&other) : q(other.q), chained(other.chained) {       // dispatcher实例可以移动
            other.chained = true;                                                  // 源不能等待消息
        }

        explicit dispatcher(queue *q_) :
                q(q_), chained(false) {}

        template<typename Message, typename Func>
        TemplateDispatcher<dispatcher, Message, Func>
        handle(Func &&f) {                                                          // 3 使用TemplateDispatcher处理指定类型的消息
            return TemplateDispatcher<dispatcher, Message, Func>(
                    q, this, std::forward<Func>(f));
        }

        ~dispatcher() noexcept(false) {                                          // 4 析构函数可能会抛出异常
            if (!chained) {
                wait_and_dispatch();
            }
        }
    };
}
