

#include "EventLoop.h"
#include "Log/Logging.h"
#include "Timer/TimeStamp.h"
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>

EventLoop::EventLoop()
    :_tid(std::this_thread::get_id()),
     _eventfd(eventfd(0, EFD_CLOEXEC)),
     _eventfdChannel(nullptr),
     _isDoingPending(false),
     _epoll(new Epoll()),
     _timerQueue(new TimerQueue(this))
{
    assert(_epoll);
    assert(_eventfd > 0);
    assert(_timerQueue);
    _eventfdChannel = new Channel(this, _eventfd);

    _eventfdChannel->setReadCALLBACK(   // asyncWakeUp by other thread
        [this]()->void{

            std::size_t num;
            read(this->_eventfd, &num, sizeof(std::size_t));

        }
    );

    _eventfdChannel->setWriteCALLBACK(
        [this]()->void{
            // do nothing
        }
    );

    _eventfdChannel->setWatchEvent(EPOLLIN);
    _eventfdChannel->addToLoop();

    LOG_INFO << "thread = " << _tid << " construct a eventLoop, TIMEOUT = " << TIMEOUT << ", MAXLOOPTIME = " << MAXLOOPTIME;
}

EventLoop::~EventLoop(){

    for(auto& c : _fd2channel) delete c.second;

    if(_epoll) delete _epoll;

    if(_timerQueue) delete _timerQueue;
}

void EventLoop::loop(){

    std::size_t maxLoopTimes = MAXLOOPTIME;
    if(tag) LOG_TRACE << TimeStamp::now().whenCreate_str() << " thread = " << _tid << " start loop";

    while(maxLoopTimes --){
        std::vector<Channel*> channels;

        std::size_t triggerd = _epoll->wait(channels);

        if(channels.size()){
            maxLoopTimes = MAXLOOPTIME;

            for(auto& p : channels){
 
                if(p->getReturnEvent() & EPOLLIN) p->runReadCallback();
                
                if(p->getReturnEvent() & EPOLLOUT) p->runWriteCallBack();
            }
        } 

        std::list<CALLBACK> temp;

        {
            std::lock_guard<std::mutex> g_lock(_mt_pendingQueue);
            temp.swap(_pendingQueue);
        }

        _isDoingPending = true;

        for(auto& timerCallBack : temp) timerCallBack(); 

        _isDoingPending = false;
    }

    LOG_WARN << TimeStamp::now().whenCreate_str() << " At Least continuous " << MAXLOOPTIME << " loop NO fd is triggered";
}

void EventLoop::runInLoop(CALLBACK&& callback){

    if(std::this_thread::get_id() == _tid) callback();
    else{

        LOG_DEBUG << TimeStamp::now().whenCreate_str() << " thread = " << std::this_thread::get_id() << " enter threadloop which _tid = " << _tid;
        queueInLoop(std::move(callback));
    }

}

void EventLoop::queueInLoop(CALLBACK&& callback){

    {
        std::lock_guard<std::mutex> g_lock(this->_mt_pendingQueue);
        _pendingQueue.push_back(std::move(callback));
    }

    if(std::this_thread::get_id() != _tid || _isDoingPending){
        asyncWakeUp();
    }
}

void EventLoop::asyncWakeUp(){
    std::size_t num = 1;
    std::size_t n = write(_eventfd, &num, sizeof(std::size_t));
    if(tag) LOG_TRACE << TimeStamp::now().whenCreate_str() << " thread = " << std::this_thread::get_id() << " asyncWakeUp thread = " << _tid 
                        << " by write eventfd " << _eventfd << " " << n << " bytes";
}