
#ifndef CHANNEL_H
#define CHANNEL_H

#include <cstddef>
#include <unistd.h>
#include <functional>

class EventLoop;

class Channel{
public:
    typedef std::function<void(void)> CALLBACK;

    explicit Channel(EventLoop* loop, std::size_t fd);
    ~Channel(){
        if(_fd > 0) close(_fd);
    }

    void setReadCALLBACK(CALLBACK&& callback){ _EPOLLIN_CALLBACK = callback; }
    void setWriteCALLBACK(CALLBACK&& callback){ _EPOLLOUT_CALLBACK = callback; }

    void runReadCallback() { _EPOLLIN_CALLBACK(); }

    void runWriteCallBack() { _EPOLLOUT_CALLBACK(); }

    void setWatchEvent(std::size_t event);

    void addWatchEvent(std::size_t event);

    void removeWatchEvent(std::size_t event);

    void setReturnEvent(std::size_t revent);

    void addToLoop();

    void removeInloop();

    std::size_t getFD(){ return _fd; }

    std::size_t getWatchEvent(){ return _event; }

    std::size_t getReturnEvent(){ return _revent; }

private:
    CALLBACK _EPOLLIN_CALLBACK;
    CALLBACK _EPOLLOUT_CALLBACK;

    EventLoop* _loop;

    std::size_t _fd;

    std::size_t _event;

    std::size_t _revent;

    bool _inLoop;

    static void defaultReadFUNC(std::size_t fd);

    static void defaultWriteFUNC(std::size_t fd);

};

#endif