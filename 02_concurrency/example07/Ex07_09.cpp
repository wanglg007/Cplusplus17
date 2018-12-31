//interruptible_thread的基本实现
class interrupt_flag {
public:
    void set();

    bool is_set() const;
};

thread_local interrupt_flag this_thread_interrupt_flag;     // 1

class interruptible_thread {
    std::thread internal_thread;
    interrupt_flag *flag;
public:
    template<typename FunctionType>
    interruptible_thread(FunctionType f) {
        std::promise < interrupt_flag * > p;                 // 2
        internal_thread = std::thread([f, &p] {              // 3
            p.set_value(&this_thread_interrupt_flag);
            f();                                             // 4
        });
        flag = p.get_future().get();                         // 5
    }

    void interrupt() {
        if (flag) {
            flag->set();                                     // 6
        }
    }
};

/**
 * The supplied function f is wrapped in a lambda function (3) that holds a copy of f and a reference to the local promise p (2).
 * The lambda sets the value of the promise to the address of the this_thread_interrupt_flag (which is declared thread_local (1))
 * for the new thread before invoking the copy of the supplied function (4). The calling thread then waits for the future associated
 * with the promise to become ready and stores the result in the flag member variable (5). Note that even though the lambda is
 * running on the new thread and has a dangling reference to the local variable p, this is OK because the interruptible_thread
 * constructor waits until p is no longer referenced by the new thread before returning. Note that this implementation doesn’t
 * take account of handling joining with the thread, or detaching it. You need to ensure that the flag variable is cleared when
 * the thread exits, or is detached, to avoid a dangling pointer.
 *
 * The interrupt() function is then relatively straightforward: if you have a valid pointer to an interrupt flag, you have a thread
 * to interrupt, so you can just set the flagg (6). It’s then up to the interrupted thread what it does with the interruption.
 *
 */