
#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <functional>
#include <unordered_map>
#include <list>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "Channel.h"
#include "Epoll.h"
#include "TimerQueue.h"
#include "Define.h"

#ifndef MAXLOOPTIME
    #define MAXLOOPTIME (1)
#endif

class EventLoop{

public:
    typedef std::function<void(void)> CALLBACK;

    explicit EventLoop();
    ~EventLoop();

    void loop();

    void runInLoop(CALLBACK&& runTask);

    void queueInLoop(CALLBACK&& pendingTask);

    void updateWatchEvent(std::size_t fd){
        _epoll->update(_fd2channel[fd]);
    }

    void updateWatchEvent(Channel* channel){
        _epoll->update(channel);
    }

    void addChannel(Channel* channel){
        _epoll->add(channel);
    }

    void removeChannel(std::size_t fd){
        _epoll->remove(_fd2channel[fd]);
        _fd2channel.erase(fd);
    }

    void removeChannel(Channel* channel){
        _epoll->remove(channel);
        _fd2channel.erase(channel->getFD());
    }

    void addMapping(Channel* channel){ _fd2channel[channel->getFD()] = channel; }

    const std::thread::id getTid() const{ return _tid; }


    void runAt(TimeStamp when, CALLBACK&& callback) {
        _timerQueue->insertTimerEvent(when, 0, std::move(callback));
    }

    void runAfter(std::size_t delay, CALLBACK&& callback) {
        _timerQueue->insertTimerEvent(TimeStamp::nowAfter(delay), 0, std::move(callback));
    }

    void runEvery(std::size_t interval, CALLBACK&& callback, std::size_t count = -1) {
        _timerQueue->insertTimerEvent(TimeStamp::now(), interval, std::move(callback), count);
    }


private:

    std::unordered_map<std::size_t, Channel*> _fd2channel;

    Epoll* _epoll;

    std::list<CALLBACK> _pendingQueue;

    std::mutex _mt_pendingQueue;

    std::condition_variable _cond_pendingQueue;

    const std::thread::id _tid;

    const std::size_t _eventfd;

    Channel* _eventfdChannel;

    void asyncWakeUp();

    std::atomic<bool> _isDoingPending;

    TimerQueue* _timerQueue;
};


#endif