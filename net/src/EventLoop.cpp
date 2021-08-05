
/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:09:58
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:58:01
 */

#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <cassert>
#include <unistd.h>

#include "EventLoop.h"
#include "Logging.h"
#include "TimeStamp.h"

EventLoop::EventLoop()
    :_tid(get_thread_id()),
     _eventfd(eventfd(0, EFD_CLOEXEC)),
     _eventfdChannel(nullptr),
     _isDoingPending(false),
     _epoll(new Epoll()),
     _timerQueue(new TimerQueue(this))
{
    assert(_epoll.get());
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

    LOG_INFO << "New eventLoop constructed by thread_id=[" << _tid << "]";
}

EventLoop::~EventLoop(){

    for(auto& c : _fd2channel) delete c.second;  // 所有fd的channel：listenfd，timerfd，eventfd

    if(_timerQueue) delete _timerQueue;          // TimerQueue中创建的timerfd的channel[TcpConn]由EventLoop管理

    LOG_WARN << TimeStamp::now().whenCreate_str() << " eventLoop deconstruct with thread_id=[" << _tid << "]";
}

void EventLoop::loop(){
    if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " eventLoop start loop with thread_id=[" << _tid << "]";

    while(true){
        std::vector<Channel*> channels;
        std::size_t triggerd = _epoll->wait(channels);

        if(channels.size()){
            for(auto& p : channels){
                if(p->getReturnEvent() & EPOLLIN) p->runReadCallback();
                if(p->getReturnEvent() & EPOLLOUT) p->runWriteCallBack();
            }
        } 

        std::list<TASK_CALLBACK> temp;

        {
            std::lock_guard<std::mutex> g_lock(_mt_pendingQueue);
            temp.swap(_pendingQueue);
        }

        _isDoingPending = true;

        for(auto& timerCallBack : temp) timerCallBack(); 

        _isDoingPending = false;
    }
}

void EventLoop::runInLoop(TASK_CALLBACK&& callback){

    std::size_t cur_thread_id = get_thread_id();
    if(cur_thread_id == _tid) callback();
    else{
        LOG_DEBUG << TimeStamp::now().whenCreate_str() << " thread_id=[" << cur_thread_id << "] enter threadloop created by thread_id=[" << _tid << "]";
        queueInLoop(std::move(callback));
    }

}

void EventLoop::queueInLoop(TASK_CALLBACK&& callback){

    {
        std::lock_guard<std::mutex> g_lock(this->_mt_pendingQueue);
        _pendingQueue.push_back(std::move(callback));
    }

    if(get_thread_id() != _tid || _isDoingPending){
        asyncWakeUp();
    }
}

void EventLoop::asyncWakeUp(){
    std::size_t num = 1;
    std::size_t n = write(_eventfd, &num, sizeof(std::size_t));
    if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " thread_id=[" << get_thread_id() << "] asyncWakeUp thread_id=[" << _tid << "]";
}