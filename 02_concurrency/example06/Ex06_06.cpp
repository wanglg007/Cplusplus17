//将GUI线程和任务线程进行分离
std::thread task_thread;
std::atomic<bool> task_cancelled(false);

void gui_thread() {
    while (true) {
        event_data event = get_event();
        if (event.type == quit)
            break;
        process(event);
    }
}

void task() {
    while (!task_complete() && !task_cancelled) {
        do_next_operation();
    }
    if (task_cancelled) {
        perform_cleanup();
    } else {
        post_gui_event(task_complete);
    }
}

void process(event_data const &event) {
    switch (event.type) {
        case start_task:
            task_cancelled = false;
            task_thread = std::thread(task);
            break;
        case stop_task:
            task_cancelled = true;
            task_thread.join();
            break;
        case task_complete:
            task_thread.join();
            display_results();
            break;
        default:
            //...
    }
}

/**
 * By separating the concerns in this way, the user thread is always able to respond to the events in a timely fashion, even if
 * the task takes a long time. This responsiveness is often key to the user experience when using an application; applications
 * that completely lock up whenever a particular operation is being performed (whatever that may be) are inconvenient to use. By
 * providing a dedicated event-handling thread, the GUI can handle GUI-specific messages (such as resizing or repainting the window)
 * without interrupting the execution of the time-consuming processing, while still passing on the relevant messages where they
 * do affect the long-running task.
 *
 */