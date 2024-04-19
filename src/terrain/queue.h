#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <glm_includes.h>

// Subclass of std::queue with a method for finding
// existing items.
template<typename T>
class Queue : public std::queue<T> {
public:
    bool exists(const T &item) const {
        auto it = std::find(this->c.begin(), this->c.end(), item);
        return it != this->c.end();
    }
};

// A thread-safe, general purpose queue.
template<typename T>
class TerrainWorkQueue {
public:
    TerrainWorkQueue() {}

    TerrainWorkQueue(TerrainWorkQueue const &other) {
        std::lock_guard<std::mutex> lk(other.m_mutex);
        m_dataQueue = other.m_dataQueue;
    }

    void push(T item) {
        std::lock_guard<std::mutex> lk(m_mutex);
        m_dataQueue.push(item);
        m_condition.notify_one();
    }

    void pushUnique(T item) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (!m_dataQueue.exists(item)) {
            m_dataQueue.push(item);
            m_condition.notify_one();
        }
    }

    // Pop an item from the queue, if there's any, without waiting.
    bool tryPop(T &poppedItem) {
        std::lock_guard<std::mutex> lk(m_mutex);
        if (m_dataQueue.empty()) {
            return false;
        }
        poppedItem = m_dataQueue.front();
        m_dataQueue.pop();
        return true;
    }

    // Pop an item from the queue, waiting until there's one.
    void waitAndPop(T &poppedItem) {
        std::unique_lock<std::mutex> lk(m_mutex);
        m_condition.wait(
            lk,
            [this]{return !m_dataQueue.empty();}
        );
        poppedItem = m_dataQueue.front();
        m_dataQueue.pop();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(m_mutex);
        return m_dataQueue.empty();
    }

private:
    mutable std::mutex m_mutex;

    Queue<T> m_dataQueue;

    std::condition_variable m_condition;
};
