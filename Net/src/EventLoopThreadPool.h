

#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <atomic>
#include "EventLoop.h"

class EventLoopThreadPool{

public:
    explicit EventLoopThreadPool(EventLoop* baseLoop, std::size_t num = 2);
    ~EventLoopThreadPool();

    void start();

    void stop();

    EventLoop* getNextLoop();

    std::size_t getThreadNum(){ return _num; }

private:

    std::size_t _current;

    std::vector<std::thread> _threads;

    std::size_t _num;

    EventLoop* _baseLoop;

    std::vector<EventLoop*> _loops;

    std::mutex _mt_loops;

    std::atomic<bool> _sync;
};


#endif