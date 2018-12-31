//为std::condition_variable_any设计的interruptible_wait
class interrupt_flag {
    std::atomic<bool> flag;
    std::condition_variable *thread_cond;
    std::condition_variable_any *thread_cond_any;
    std::mutex set_clear_mutex;

public:
    interrupt_flag() : thread_cond(0), thread_cond_any(0) {}

    void set() {
        flag.store(true, std::memory_order_relaxed);
        std::lock_guard <std::mutex> lk(set_clear_mutex);
        if (thread_cond) {
            thread_cond->notify_all();
        } else if (thread_cond_any) {
            thread_cond_any->notify_all();
        }
    }

    template<typename Lockable>
    void wait(std::condition_variable_any &cv, Lockable &lk) {
        struct custom_lock {
            interrupt_flag *self;
            Lockable &lk;

            custom_lock(interrupt_flag *self_, std::condition_variable_any &cond, Lockable &lk_) :
                    self(self_), lk(lk_) {
                self->set_clear_mutex.lock();       // 1
                self->thread_cond_any = &cond;      // 2
            }

            void unlock() {                        // 3
                lk.unlock();
                self->set_clear_mutex.unlock();
            }

            void lock() {
                std::lock(self->set_clear_mutex, lk);   // 4
            }

            ~custom_lock() {
                self->thread_cond_any = 0;              // 5
                self->set_clear_mutex.unlock();
            }
        };

        custom_lock cl(this, cv, lk);
        interruption_point();
        cv.wait(cl);
        interruption_point();
    }

    // rest as before
};

template<typename Lockable>
void interruptible_wait(std::condition_variable_any &cv,
                        Lockable &lk) {
    this_thread_interrupt_flag.wait(cv, lk);
}

/**
 * Your custom lock type acquires the lock on the internal set_clear_mutex when it’s constructed (1) and then sets the
 * thread_cond_any pointer to refer to the std::condition_variable_any passed in to the constructor (2). The Lockable
 * reference is stored for later; this must already be locked. You can now check for an interruption without worrying
 * about races. If the interrupt flag is set at this point, it was set before you acquired the lock on set_clear_mutex.
 * When the condition variable calls your unlock() function inside wait(),you unlock the Lockable object and the internal
 * set_clear_mutex (3). This allows threads that are trying to interrupt you to acquire the lock on set_clear_mutex and
 * check the thread_cond_any pointer once you’re inside the wait() call but not before. This is exactly what you were
 * after (but couldn’t manage) with std::condition_variable. Once wait() has finished waiting (either because it was
 * notified or because of a spurious wake),it will call your lock() function, which again acquires the lock on the internal
 * set_clear_mutex and the lock on the Lockable object (4). You can now check again for interruptions that happened during
 * the wait() call before clearing the thread_cond_any pointer in your custom_lock destructor (5), where you also unlock
 * the set_clear_mutex.
 */
