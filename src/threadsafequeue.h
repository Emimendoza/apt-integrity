#ifndef APT_INTEGRITY_THREADSAFEQUEUE_H
#define APT_INTEGRITY_THREADSAFEQUEUE_H

#include <queue>
#include <mutex>

template<typename T>
class threadsafequeue
{
public:
private:
    std::queue<T> queue;
    std::mutex lock;
};


#endif //APT_INTEGRITY_THREADSAFEQUEUE_H
