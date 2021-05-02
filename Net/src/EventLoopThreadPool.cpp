

#include "EventLoopThreadPool.h"
#include "Log/Logging.h"
#include "Timer/TimeStamp.h"
#include <cassert>

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

        loop = _loops[_current];
        _current = (_current+1) % _num;

    }   

    return loop;
}

void EventLoopThreadPool::start(){
    for(int i = 0; i<_num; ++i){

        _threads.push_back(std::thread(

            [this]()->void{

                EventLoop* loop = new EventLoop();

                {
                    std::lock_guard<std::mutex> g_lock(this->_mt_loops);

                    this->_loops.push_back(loop);
                }

                loop->loop();

                delete loop;

            }

        ));

        LOG_INFO << TimeStamp::now().whenCreate_str() << " thread = " << std::this_thread::get_id() << " create a ioLoop";
    }
}

void EventLoopThreadPool::stop(){
    _sync.store(true, std::memory_order_release);
}