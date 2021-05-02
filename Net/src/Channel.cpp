
#include "Channel.h"
#include "EventLoop.h"
#include "Log/Logging.h"
#include <iostream>

Channel::Channel(EventLoop* loop, std::size_t fd)
    :_loop(loop),
     _fd(fd),
     _event(0),
     _revent(0),
     _inLoop(false),
     _EPOLLIN_CALLBACK(std::bind(Channel::defaultReadFUNC, _fd)),
     _EPOLLOUT_CALLBACK(std::bind(Channel::defaultWriteFUNC, _fd))
{
    _loop->addMapping(this);
}

void Channel::setWatchEvent(std::size_t event) {
    _event = event;
    if(_inLoop) _loop->updateWatchEvent(this);
}

void Channel::addWatchEvent(std::size_t event){ 
    _event |= event;
    if(_inLoop) _loop->updateWatchEvent(this);
}

void Channel::removeWatchEvent(std::size_t event) { 
    _event &= ~event; 
    if(_inLoop) _loop->updateWatchEvent(this);
}


void Channel::setReturnEvent(std::size_t revent){ 
    _revent = revent;
}

void Channel::addToLoop(){ 
    _inLoop = true;
    _loop->addChannel(this); 
}

void Channel::removeInloop(){
    _inLoop = false;
    _loop->removeChannel(this);
}



void Channel::defaultReadFUNC(std::size_t fd){
    LOG_WARN << TimeStamp::now().whenCreate_str() << ", fd = " << fd << " default ReadCallBack" ;
}

void Channel::defaultWriteFUNC(std::size_t fd){
    LOG_WARN << TimeStamp::now().whenCreate_str() << ", fd = " << fd << " default WriteCallBack" ;
}