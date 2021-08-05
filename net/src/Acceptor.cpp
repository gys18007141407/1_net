/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 17:02:15
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:57:35
 */

#include <cassert>
#include "Acceptor.h"
#include "Logging.h"
#include "TimeStamp.h"
#include "EventLoop.h"
#include "Channel.h"



Acceptor::Acceptor(EventLoop* loop)
    :_listenChannel(nullptr),
     _listenLoop(loop)
{

    _listenfd = socket(AF_INET, SOCK_STREAM, 0);

    assert(_listenfd > 0);

    setFdReuse(_listenfd);

}

Acceptor::~Acceptor(){
    if(_listenChannel) delete _listenChannel;
}

bool Acceptor::init(){

    _sockaddr = _addr.getSockAddr();

    if(-1 == bind(_listenfd, (sockaddr*)&_sockaddr, sizeof(sockaddr))){
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " bind failed, " << strerror(errno) 
                  << ", current IP=[" << _addr.getIP() << "], current port=[" << _addr.getPort() 
                  << "], current htonsPort=[" << _addr.getHtonsPort() << "]";
        return false;
    }
    return true;
}

void Acceptor::start(){
    if(!init() || -1 == listen(_listenfd, LISTENQUEUE)){
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " listen failed, " << strerror(errno) 
                  << ", current IP=[" << _addr.getIP() << "], current port=[" << _addr.getPort() 
                  << "], current htonsPort=[" << _addr.getHtonsPort() << "]";
        return;
    }

    LOG_INFO << TimeStamp::now().whenCreate_str() << " server listening..., current IP=[" 
             << _addr.getIP() << "], current port=[" << _addr.getPort() 
             << "], current htonsPort=[" << _addr.getHtonsPort() << "]";
    
    _listenChannel = new Channel(_listenLoop, _listenfd);

    _listenChannel->setWatchEvent(EPOLLIN);
    _listenChannel->setReadCALLBACK(
        [this]()->void{
            sockaddr_in addr;
            socklen_t len = sizeof(sockaddr);
            std::size_t fd = accept(this->_listenfd, (sockaddr*)&addr, &len);

            if(fd > 0) {

                LOG_INFO << TimeStamp::now().whenCreate_str() << " accept a new connection with fd=[" << fd << "]";
                this->_accept_callback(fd);

            }else LOG_WARN << TimeStamp::now().whenCreate_str() << " accept failed, " << strerror(errno);
        }
    );

    _listenChannel->addToLoop();
    _listenLoop->loop();
}


