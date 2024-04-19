#include "threadjoiner.h"

// Joins all the threads.
ThreadJoiner::~ThreadJoiner() {
    for (unsigned long i = 0; i < m_threads.size(); ++i) {
        if (m_threads[i].joinable()) {
            m_threads[i].join();
        }
    }
}
