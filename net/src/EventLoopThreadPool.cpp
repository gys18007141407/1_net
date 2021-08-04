/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 19:04:54
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:59:06
 */



#include <cassert>

#include "EventLoopThreadPool.h"
#include "Logging.h"
#include "TimeStamp.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, std::size_t num)
    :_current(0),
     _baseLoop(baseLoop),
     _sync(false)
{
    assert(_baseLoop);

    if(num > 10) num = 10;

    _num = num;

}

EventLoopThreadPool::~EventLoopThreadPool(){
    stop();
}

EventLoop* EventLoopThreadPool::getNextLoop(){ 

    EventLoop* loop = _baseLoop;

    if(_num){

        loop = _loops[_current].get();
        _current = (_current+1) % _num;

    }   

    return loop;
}

void EventLoopThreadPool::start(){
    for(int i = 0; i<_num; ++i){

        _threads.push_back(std::thread(

            [this]()->void{

                SptrEventLoop loop = std::make_shared<EventLoop>();

                {
                    std::lock_guard<std::mutex> g_lock(this->_mt_loops);

                    this->_loops.push_back(loop);
                }

                loop->loop();  // 即使被析构，this->_loops中还有一个引用。因此最终的生命周期被listen_eventloop管理

            }

        ));

        LOG_INFO << TimeStamp::now().whenCreate_str() << " thread=[" << get_thread_id() << "] create a io EventLoop";
    }
}

void EventLoopThreadPool::stop(){                   // 暂时留着，不知道有啥用
    _sync.store(true, std::memory_order_release);
}