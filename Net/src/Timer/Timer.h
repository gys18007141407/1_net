
#ifndef TIMER_H
#define TIMER_H

#include "TimeStamp.h"
#include <functional>
#include <future>
#include <atomic>

class Timer{

public:

    typedef std::function<void(void)> CALLBACK;

    explicit Timer(TimeStamp timeStamp, std::size_t interval = 0, CALLBACK&& callback = {}, std::size_t count = -1);

    ~Timer() = default;

    bool isRepeat(){ return _interval > 0;}

    void run(){ _callback(); }

    void moveInterval();

    void unRepeat(){ _interval = 0; }

    void setCallBack(CALLBACK&& callback){ _callback = callback; }

    TimeStamp getTimeStamp(){ return _timeStamp; }

private:

    TimeStamp _timeStamp;

    std::size_t _interval;

    CALLBACK _callback;

    std::atomic<std::size_t> _count;


private:

    Timer(const Timer&) = delete;
    Timer(Timer&&) = delete;
    Timer& operator=(const Timer&) = delete;
    Timer& operator=(Timer&&) = delete;

};


#endif