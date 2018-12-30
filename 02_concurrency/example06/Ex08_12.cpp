//简单的栅栏类
class barrier {
    unsigned const count;
    std::atomic<unsigned> spaces;
    std::atomic<unsigned> generation;
public:
    explicit barrier(unsigned count_) :                 // 1
            count(count_), spaces(count), generation(0) {}

    void wait() {
        unsigned const my_generation = generation;      // 2
        if (!--spaces) {                                  // 3
            spaces = count;                               // 4
            ++generation;                                 // 5
        } else {
            while (generation == my_generation)          // 6
                std::this_thread::yield();                // 7
        }
    }
};

/**
 * With this implementation, you construct a barrier with the number of “seats” (1),which is stored in the count variable.
 * Initially, the number of spaces at the barrier is equal to this count. As each thread waits, the number of spaces is
 * decremented (3).When it reaches zero, the number of spaces is reset back to count (4), and the generation is increased
 * to signal to the other threads that they can continue (5). If the number of free spaces does not reach zero, you have
 * to wait. This implementation uses a simple spin lock (6), checking the generation against the value you retrieved at
 * the beginning of wait() (2). Because the generation is only updated when all the threads have reached the barrier (5),
 * you yield() while waiting (7) so the waiting thread doesn’t hog the CPU in a busy wait.
 *
 */