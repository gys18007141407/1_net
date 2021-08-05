/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:09:58
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:58:01
 */


#include <cassert>
#include <unistd.h>

#include "Epoll.h"
#include "Channel.h"
#include "Logging.h"
#include "TimeStamp.h"

Epoll::Epoll()
    :_epollfd(epoll_create1(EPOLL_CLOEXEC))   // epollfd关闭时， 关闭所有fd

{
    assert(_epollfd > 0);
    if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " create epollfd " << _epollfd << " by thread " << std::this_thread::get_id();
}

Epoll::~Epoll(){
    if(_epollfd > 0) close(_epollfd);
}

std::size_t Epoll::wait(std::vector<Channel*>& channels){

    std::size_t n = epoll_wait(_epollfd, _epollEvents, MAXEVENTS, TIMEOUT);

    for(int i = 0; i<n; ++i){

        channels.push_back( static_cast<Channel*> (_epollEvents[i].data.ptr));
        channels.back()->setReturnEvent(_epollEvents[i].events);

        if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd = " << channels.back()->getFD() << " triggered " << channels.back()->getReturnEvent() << " event by epollfd " << _epollfd;
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
        LOG_WARN << TimeStamp::now().whenCreate_str() << " thread_id=[" << get_thread_id() << "] remove fd=[" 
                 << channel->getFD() << "] from epollfd=[" << _epollfd << "] failed!";
    }
}

void Epoll::update(Channel* channel){
    _ev.data.ptr = (void*)channel;
    _ev.events = channel->getWatchEvent();

    if(-1 == epoll_ctl(_epollfd, EPOLL_CTL_MOD, channel->getFD(), &_ev)){
        LOG_WARN << TimeStamp::now().whenCreate_str() << " thread_id=[" << get_thread_id() << "] modify fd=[" 
                 << channel->getFD() << "] from epollfd=[" << _epollfd << "] failed!";
    }
}