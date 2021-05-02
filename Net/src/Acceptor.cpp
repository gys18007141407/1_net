
#include <cassert>
#include "Acceptor.h"
#include "Log/Logging.h"
#include "Timer/TimeStamp.h"
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
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " bind failed, " << strerror(errno) << ", current IP is " 
                << _addr.getIP() << ", current port is " << _addr.getPort() << ", current htonsPort is " << _addr.getHtonsPort();
        return false;
    }
    return true;
}

void Acceptor::start(){
    if(!init() || -1 == listen(_listenfd, LISTENQUEUE)){
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " listen failed, " << strerror(errno) << ", current IP is " << _addr.getIP() << ", current port is " << _addr.getPort();
        return;
    }

    LOG_INFO << TimeStamp::now().whenCreate_str() << " listen ... , current IP is " << _addr.getIP() << ", current port is " 
            << _addr.getPort() << ", current htonsPort is " << _addr.getHtonsPort() << ", LISTENQUEUE = " << LISTENQUEUE;

    
    _listenChannel = new Channel(_listenLoop, _listenfd);

    _listenChannel->setWatchEvent(EPOLLIN);
    _listenChannel->setReadCALLBACK(
        [this]()->void{
            sockaddr_in addr;
            socklen_t len = sizeof(sockaddr);
            std::size_t fd = accept(this->_listenfd, (sockaddr*)&addr, &len);

            if(fd > 0) {

                LOG_INFO << TimeStamp::now().whenCreate_str() << " accept a new connection fd = " << fd ;
                this->_accept_callback(fd);

            }else LOG_WARN << TimeStamp::now().whenCreate_str() << " accept failed, " << strerror(errno);
        }
    );

    _listenChannel->addToLoop();
    _listenLoop->loop();
}

void Acceptor::setFdReuse(std::size_t fd){
    std::size_t reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(std::size_t));  //param2: SOL_SOCKET、IPPROTO_TCP、IPPROTO_IP、IPPROTO_IPV6。
}
