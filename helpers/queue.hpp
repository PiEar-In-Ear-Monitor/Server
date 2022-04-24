#ifndef PIEAR_SERVER_QUEUE_HPP
#define PIEAR_SERVER_QUEUE_HPP

#include <queue>
#include <mutex>

template<typename T>
class queue {
public:
    queue() {}
    void push(T *item) {
        T forQueue;
        forQueue = std::move(*item);
        std::lock_guard<std::mutex> lock(m);
        q.push(forQueue);
    }
    T pop() {
        if (q.empty()) {
            return nullptr;
        }
        std::lock_guard<std::mutex> lock(m);
        T item = q.front();
        q.pop();
        return item;
    }
private:
    std::queue<T> q;
    std::mutex m;
};

#endif //PIEAR_SERVER_QUEUE_HPP
