
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <string>
#include <cstddef>
#include "IServer.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"

class EventLoop;

class TcpServer{

public:

    explicit TcpServer();
    explicit TcpServer(const char* IP, const std::size_t port);
    explicit TcpServer(const std::string IP, const std::size_t port);
    explicit TcpServer(const std::size_t threads);
    explicit TcpServer(const char* IP, const std::size_t port, const std::size_t threads);
    explicit TcpServer(const std::string IP, const std::size_t port, const std::size_t threads);

    ~TcpServer();

    void start();

    std::size_t getThreadNum(){ return _ioLoop->getThreadNum(); }



    void setServer(IServer* server){ _server = server; }



    void setIP(const char* IP){ _acceptor->setIP(IP); }

    void setIP(std::string IP) { _acceptor->setIP(IP); }

    void setPort(std::size_t port){ _acceptor->setPort(port); }

private:

    EventLoop* _listenLoop;

    EventLoopThreadPool* _ioLoop;

    void acceptCallBackFunc(std::size_t fd);

    Acceptor* _acceptor;

    IServer* _server;
};


#endif