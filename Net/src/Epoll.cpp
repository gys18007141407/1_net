

#include "Epoll.h"
#include "Channel.h"
#include "Log/Logging.h"
#include "Timer/TimeStamp.h"
#include <cassert>
#include <unistd.h>

Epoll::Epoll()
    :_epollfd(epoll_create1(EPOLL_CLOEXEC))

{
    assert(_epollfd > 0);
    if(tag) LOG_TRACE << TimeStamp::now().whenCreate_str() << " create epollfd " << _epollfd << " by thread " << std::this_thread::get_id();
}

Epoll::~Epoll(){
    if(_epollfd > 0) close(_epollfd);
}

std::size_t Epoll::wait(std::vector<Channel*>& channels){

    std::size_t n = epoll_wait(_epollfd, _epollEvents, MAXEVENTS, TIMEOUT);

    for(int i = 0; i<n; ++i){

        channels.push_back( static_cast<Channel*> (_epollEvents[i].data.ptr));
        channels.back()->setReturnEvent(_epollEvents[i].events);

        if(tag) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd = " << channels.back()->getFD() << " triggered " << channels.back()->getReturnEvent() << " event by epollfd " << _epollfd;
    }

    return n;
}

void Epoll::add(Channel* channel){
    _ev.data.ptr = (void*)channel;
    _ev.events = channel->getWatchEvent();

    if(-1 == epoll_ctl(_epollfd, EPOLL_CTL_ADD, channel->getFD(), &_ev)){
        LOG_WARN << TimeStamp::now().whenCreate_str() << " " << std::this_thread::get_id() << " add fd " << channel->getFD() << " to epollfd " << _epollfd << " failed!";
    }
}

void Epoll::remove(Channel* channel){
    _ev.data.ptr = (void*)channel;
    _ev.events = channel->getWatchEvent();

    if(-1 == epoll_ctl(_epollfd, EPOLL_CTL_DEL, channel->getFD(), &_ev)){
        LOG_WARN << TimeStamp::now().whenCreate_str() << " " << std::this_thread::get_id() << " remove fd " << channel->getFD() << " from epollfd " << _epollfd << " failed!";
    }
}

void Epoll::update(Channel* channel){
    _ev.data.ptr = (void*)channel;
    _ev.events = channel->getWatchEvent();

    if(-1 == epoll_ctl(_epollfd, EPOLL_CTL_MOD, channel->getFD(), &_ev)){
        LOG_WARN << TimeStamp::now().whenCreate_str() << " " << std::this_thread::get_id() << " mod fd " << channel->getFD() << " in epollfd " << _epollfd << " failed!";
    }
}