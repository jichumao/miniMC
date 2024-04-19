#pragma once
#include <atomic>
#include <functional>
#include <vector>
#include <thread>
#include "queue.h"
#include "threadjoiner.h"

// A general-purpose thread pool with a work queue. The main thread submits
// functions to it and available threads from the pool grab them from the
// work queue and execute them.
class TerrainThreadPool {
public:
    // The constructor creates and starts all threads.
    TerrainThreadPool();

    ~TerrainThreadPool();

    // Places the given function in the work queue.
    void submit(std::function<void()> f);

    inline bool isDone() const {
        return m_done;
    }

private:
    // Indicates that all threads in the pool must exit.
    std::atomic_bool m_done;

    // A thread-safe queue of functions. The caller (usually the main thread)
    // submits functions to this queue and threads from the pool grab them
    // and execute them.
    TerrainWorkQueue<std::function<void()>> m_workQueue;

    std::vector<std::thread> m_threads;

    // Joins all the threads when m_done is true. It executes in the TerrainThreadPool
    // destructor. It's crucial that this member variable is listed here at the end
    // because member variables are destroyed in the reverse order that they are declared
    // and we want the ThreadJoiner to be destroyed before m_threads, so that it can join
    // them before the std::thread destructor executes (program terminates if an std::thread
    // destructor executes before the thread is joined or detached).
    ThreadJoiner m_joiner;

    // Main loop of a thread. Picks up functions from the work queue and executes them.
    void worker();
};
