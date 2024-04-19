#pragma once
#include <thread>
#include <vector>

// Joins all the threads when it gets destroyed. We do this because the program
// terminates if an std::thread destructor executes before the thread is joined
// or detached.
class ThreadJoiner {
public:
    explicit ThreadJoiner(
        std::vector<std::thread> &threads
    ) : m_threads(threads)
    {}

    ~ThreadJoiner();

private:
    std::vector<std::thread> &m_threads;
};
