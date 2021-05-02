

#include "TimerQueue.h"
#include "Log/Logging.h"
#include "EventLoop.h"
#include "Channel.h"
#include <sys/timerfd.h>
#include <assert.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

TimerQueue::TimerQueue(EventLoop* loop): _loop(loop){
    _tfd = timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC | TFD_NONBLOCK);
    assert(_tfd > 0);

    Channel* channel = new Channel(_loop, _tfd);

    channel->setReadCALLBACK(
        [this]()->void{
            // timerEvent triggered

            std::size_t overtimeCount;
            read(this->_tfd, &overtimeCount, sizeof(std::size_t));

            assert(overtimeCount);

            std::vector<TT> triggered = this->getOverTimeEvent(TimeStamp::now());

            for(auto& p : triggered) p.second->run();

            for(auto& p : triggered){
                if(p.second->isRepeat()){
                    p.second->moveInterval();
                    this->insertTimerPointer(p.second);
                }
            }

            TimeStamp nextTrigger;

            if(this->_TimerEvents.size()) nextTrigger = this->_TimerEvents.begin()->first;

            if(nextTrigger.isValid()) this->setTimerfd(nextTrigger);

        }
    );
    
    channel->setWriteCALLBACK(
        []()->void{
            // nothing
        }
    );

    channel->setWatchEvent(EPOLLIN);
    channel->addToLoop();
}




std::vector<TimerQueue::TT> TimerQueue::getOverTimeEvent(TimeStamp now){

    TT last = {now, (Timer*)~0};

    std::vector<TT> overtimeEvent;

    auto p = _TimerEvents.lower_bound(last);

    // if(p == _TimerEvents.end()) then all timerEvents has overtime

    overtimeEvent = std::vector<TT>(_TimerEvents.begin(), p);

    _TimerEvents.erase(_TimerEvents.begin(), p);

    return overtimeEvent;
}

void TimerQueue::setTimerfd(TimeStamp nextTrigger){

    itimerspec newTrigger, oldTrigger;
    memset(&newTrigger, 0, sizeof(itimerspec));

    newTrigger.it_value = howFarFromNow(nextTrigger);

    //newTrigger.it_interval = 1; set timerfd interval

    timerfd_settime(_tfd, 0, &newTrigger, &oldTrigger);
}

void TimerQueue::insertTimerEvent(TimeStamp timeStamp, std::size_t interval, CALLBACK&& callback, std::size_t count){

    if(count == 0) {
        LOG_WARN << TimeStamp::now().whenCreate_str() << " insert timerEvent: " << timeStamp.whenCreate_str() << " , interval = "
                        << interval << ", but count = 0 so refuse to insert!"; 
        return;
    }

    Timer* ptimer = new Timer(timeStamp, interval, std::move(callback), count);

    _loop->queueInLoop(
        [this, ptimer]()->void{

            if(this->insertTimerPointer(ptimer)) this->setTimerfd(ptimer->getTimeStamp());

        }
    );
}

bool TimerQueue::insertTimerPointer(Timer* ptimer){
    bool earliest = false;
    
    if(!_TimerEvents.size() || _TimerEvents.begin()->first > ptimer->getTimeStamp()) earliest = true;
    _TimerEvents.insert({ptimer->getTimeStamp(), ptimer});

    return earliest;

}


void TimerQueue::removeTimerEvent(Timer* ptimer){
    ptimer->unRepeat();

    _loop->queueInLoop(
        [this, ptimer]()->void{

            for(auto& p : this->_TimerEvents)
                if(p.second == ptimer){
                    this->_TimerEvents.erase(p);
                    return;
                }
        }
    );
}

timespec TimerQueue::howFarFromNow(TimeStamp nextTrigger){
    timespec res;

    memset(&res, 0, sizeof(timespec));

    ssize_t microS = nextTrigger.whenCreate() - TimeStamp::now_t();

    if(microS < 100) microS = 100;

    res.tv_sec = microS / TimeStamp::kMicroSec;
    microS %= TimeStamp::kMicroSec;

    res.tv_nsec = microS*1000;

    return res;
}