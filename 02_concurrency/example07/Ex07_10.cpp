//为std::condition_variable实现的interruptible_wait有问题版
void interruptible_wait(std::condition_variable &cv, std::unique_lock <std::mutex> &lk) {
    interruption_point();
    this_thread_interrupt_flag.set_condition_variable(cv);  // 1
    cv.wait(lk);                                            // 2
    this_thread_interrupt_flag.clear_condition_variable();  // 3
    interruption_point();
}

/**
 * Assuming the presence of some functions for setting and clearing an association of a condition variable with an interrupt
 * flag, this code is nice and simple. It checks for interruption, associates the condition variable with the interrupt_flag
 * for the current thread (1), waits on the condition variable (2), clears the association with the condition variable (3),
 * and checks for interruption again. If the thread is interrupted during the wait on the condition variable, the interrupting
 * thread will broadcast the condition variable and wake you from the wait, so you can check for interruption.Unfortunately,
 * this code is broken: there are two problems with it. The first problem is relatively obvious if you have your exception
 * safety hat on: std::condition_variable::wait() can throw an exception, so you might exit the function without removing the
 * association of the interrupt flag with the condition variable. This is easily fixed with a structure that removes the
 * association in its destructor.
 *
 * The second, less-obvious problem is that there’s a race condition. If the thread is interrupted after the initial call
 * to interruption_point(), but before the call to wait(), then it doesn’t matter whether the condition variable has been
 * associated with the interrupt flag, because the thread isn’t waiting and so can’t be woken by a notify on the condition
 * variable. You need to ensure that the thread can’t be notified between the last check for interruption and the call to
 * wait(). Without delving into the internals of std::condition_variable, you have only one way of doing that: use the mutex
 * held by lk to protect this too, which requires passing it in on the call to set_condition_variable(). Unfortunately, this
 * creates its own problems: you’d be passing a reference to a mutex whose lifetime you don’t know to another thread (the
 * thread doing the interrupting) for that thread to lock (in the call to interrupt()),without knowing whether that thread
 * has locked the mutex already when it makes the call. This has the potential for deadlock and the potential to access a
 * mutex after it has already been destroyed, so it’s a nonstarter. It would be rather too restrictive if you couldn’t
 * reliably interrupt a condition variable wait—you can do almost as well without a special interruptible_wait()—so what
 * other options do you have? One option is to put a timeout on the wait; use wait_for() rather than wait() with a small
 * timeout value (such as 1 ms). This puts an upper limit on how long the thread will have to wait before it sees the interruption
 * (subject to the tick granularity of the clock). If you do this,the waiting thread will see rather more “spurious” wakes
 * resulting from the timeout, but it can’t easily be helped.
 */
