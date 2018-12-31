//receiver类
namespace messaging {
    class receiver {
        queue q;                    // 接受者拥有对应队列
    public:
        operator sender() {       // 允许将类中队列隐式转化为一个sender队列
            return sender(&q);
        }

        dispatcher wait() {        // 等待对队列进行调度
            return dispatcher(&q);
        }
    };
}
