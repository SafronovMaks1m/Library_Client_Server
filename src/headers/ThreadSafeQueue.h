#pragma once
#include <deque>
#include <mutex>
#include <condition_variable>
#include <utility>

template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex _mutex;
    std::condition_variable _cv;
    std::deque<T> _queue;

public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    void push(T&& value) {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push_back(std::move(value));
        }
        _cv.notify_one();
    }

    void push_wait(T&& value, size_t max_size = 10) {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this, max_size]() { return _queue.size() < max_size; });
        _queue.push_back(std::move(value));
        lock.unlock();
        _cv.notify_one();
    }

    T front() const {
        std::lock_guard<std::mutex> lg(_mutex);
        if (_queue.empty()) return T();
        return _queue.front();
    }

    T pop_front() {
        std::unique_lock<std::mutex> lock(_mutex);
        if (_queue.empty())
            return T{};
        T value = std::move(_queue.front());
        _queue.pop_front();
        lock.unlock();
        _cv.notify_one();
        return value;
    }

    T wait_and_pop() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cv.wait(lock, [this]() { return !_queue.empty(); });
        T v = std::move(_queue.front());
        _queue.pop_front();
        lock.unlock();
        return v;
    }

    size_t size() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.empty();
    }

    void clear() {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.clear();
        }
        _cv.notify_all();
    }
};
