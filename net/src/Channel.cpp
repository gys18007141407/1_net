/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 20:09:22
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:57:52
 */


#include <iostream>
#include "Channel.h"
#include "EventLoop.h"
#include "Logging.h"

Channel::Channel(EventLoop* _loop, std::size_t _fd)
    :m_EventLoop(_loop),
     m_fd(_fd),
     m_event(0),
     m_revent(0),
     m_inLoop(false),
     _EPOLLIN_CALLBACK(std::bind(&Channel::defaultReadFUNC, this)),
     _EPOLLOUT_CALLBACK(std::bind(&Channel::defaultWriteFUNC, this))
{
    m_EventLoop->addMapping(this);
}

void Channel::setWatchEvent(std::size_t event) {
    m_event = event;
    if(m_inLoop){
        m_EventLoop->updateWatchEvent(this);
    }
}

void Channel::addWatchEvent(std::size_t event){ 
    m_event |= event;
    if(m_inLoop) m_EventLoop->updateWatchEvent(this);
}

void Channel::removeWatchEvent(std::size_t event) { 
    m_event &= ~event; 
    if(m_inLoop) m_EventLoop->updateWatchEvent(this);
}


void Channel::setReturnEvent(std::size_t revent){ 
    m_revent = revent;
}

void Channel::addToLoop(){ 
    m_inLoop = true;
    m_EventLoop->addChannel(this); 
}

void Channel::removeInloop(){
    m_inLoop = false;
    m_EventLoop->removeChannel(this);
}



void Channel::defaultReadFUNC(){
    LOG_WARN << TimeStamp::now().whenCreate_str() << ", fd=" << this->m_fd << "] default ReadCallBack" ;
}

void Channel::defaultWriteFUNC(){
    LOG_WARN << TimeStamp::now().whenCreate_str() << ", fd=[" << this->m_fd << "] default WriteCallBack" ;
}