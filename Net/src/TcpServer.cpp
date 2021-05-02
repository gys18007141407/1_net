
#include "TcpServer.h"
#include "Log/Logging.h"
#include <cassert>

TcpServer::TcpServer()
    :_listenLoop(new EventLoop()),
     _ioLoop(new EventLoopThreadPool(_listenLoop)),
     _acceptor(new Acceptor(_listenLoop)),
     _server(nullptr)
{

    assert(_listenLoop);
    assert(_ioLoop);
}

TcpServer::TcpServer(const char* IP, const std::size_t port)
    :_listenLoop(new EventLoop()),
     _ioLoop(new EventLoopThreadPool(_listenLoop)),
     _acceptor(new Acceptor(_listenLoop)),
     _server(nullptr)
{
    assert(_listenLoop);
    assert(_ioLoop);
}

TcpServer::TcpServer(const std::string IP, const std::size_t port)
    :_listenLoop(new EventLoop()),
     _ioLoop(new EventLoopThreadPool(_listenLoop)),
     _acceptor(new Acceptor(_listenLoop)),
     _server(nullptr)
{
    assert(_listenLoop);
    assert(_ioLoop);
}

TcpServer::TcpServer(const std::size_t threads)
    :_listenLoop(new EventLoop()),
     _ioLoop(new EventLoopThreadPool(_listenLoop, threads)),
     _acceptor(new Acceptor(_listenLoop)),
     _server(nullptr)
{

    assert(_listenLoop);
    assert(_ioLoop);
}

TcpServer::TcpServer(const char* IP, const std::size_t port, const std::size_t threads)
    :_listenLoop(new EventLoop()),
     _ioLoop(new EventLoopThreadPool(_listenLoop, threads)),
     _acceptor(new Acceptor(_listenLoop)),
     _server(nullptr)
{
    assert(_listenLoop);
    assert(_ioLoop);
}

TcpServer::TcpServer(const std::string IP, const std::size_t port, const std::size_t threads)
    :_listenLoop(new EventLoop()),
     _ioLoop(new EventLoopThreadPool(_listenLoop, threads)),
     _acceptor(new Acceptor(_listenLoop)),
     _server(nullptr)
{
    assert(_listenLoop);
    assert(_ioLoop);
}

TcpServer::~TcpServer(){

    if(_acceptor) delete _acceptor;
    if(_listenLoop) delete _listenLoop;
    if(_ioLoop) delete _ioLoop;

}

void TcpServer::start(){

    assert(_server);
    _acceptor->setAcceptCALLBACK(std::bind(&TcpServer::acceptCallBackFunc, this, std::placeholders::_1));
    _ioLoop->start();
    
    try{
        _acceptor->start();
    }
    catch(std::exception& e){

        std::perror(e.what());
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " " << e.what();
    }

}







void TcpServer::acceptCallBackFunc(std::size_t fd){

    EventLoop* ioLoop = _ioLoop->getNextLoop();
    TcpConn* tcpConn = new TcpConn(ioLoop, fd);
    tcpConn->setServer(_server);

    _server->OnConnect(tcpConn);

}