
#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include "Timer/Timer.h"

#include <set>
#include <vector>

class EventLoop;

class TimerQueue{

public:
    typedef Timer::CALLBACK CALLBACK;
    typedef std::pair<TimeStamp, Timer*> TT;

    explicit TimerQueue(EventLoop* loop);

    ~TimerQueue() = default;

    std::vector<TT> getOverTimeEvent(TimeStamp now);

    void insertTimerEvent(TimeStamp timeStamp, std::size_t interval, CALLBACK&& callback, std::size_t count = -1);

    std::size_t getTimerfd(){
        return _tfd;
    }


private:

    std::set<TT> _TimerEvents;

    EventLoop* _loop;

    std::size_t _tfd;


    timespec howFarFromNow(TimeStamp nextTrigger);

    void setTimerfd(TimeStamp nextTrigger);
  
    void removeTimerEvent(Timer* ptimer);

    bool insertTimerPointer(Timer* ptimer);

};



#endif