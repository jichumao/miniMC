#include "threadpool.h"

// The constructor creates and starts all threads.
TerrainThreadPool::TerrainThreadPool()
    : m_done(false),
      m_joiner(m_threads)
{
    unsigned const threadCount = std::thread::hardware_concurrency();

    try {
        for(unsigned i = 0; i < threadCount; ++i) {
            m_threads.push_back(
                std::thread(&TerrainThreadPool::worker, this)
            );
        }
    } catch (...) {
        m_done = true;
        throw;
    }
}

TerrainThreadPool::~TerrainThreadPool() {
    // Signal all threads to exit.
    m_done = true;
}

// Places the given function in the work queue.
void TerrainThreadPool::submit(std::function<void()> f) {
    m_workQueue.push(f);
}

// Main loop of a thread. Picks up functions from the work queue and executes them.
void TerrainThreadPool::worker() {
    while (!m_done) {
        std::function<void()> task;

        // No busy waiting. Try to pop a function from the queue; suspend if there
        // isn't any.
        if (m_workQueue.tryPop(task)) {
            task();
        } else {
            // No work to do. Suspend the thread for a while and allow other
            // threads to be scheduled sooner.
            std::this_thread::yield();
        }
    }
}
