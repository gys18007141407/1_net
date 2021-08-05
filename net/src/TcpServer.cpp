/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 17:06:35
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:59:17
 */
#include "TcpServer.h"
#include "Logging.h"
#include <cassert>


TcpServer::TcpServer(EventLoop* pLoop, const std::string& IP, const std::size_t port, const std::size_t threads)
    :m_listenEventLoop(pLoop),
     m_Uptr_ioEventLoops(new EventLoopThreadPool(m_listenEventLoop, threads)),
     m_Uptr_Acceptor(new Acceptor(m_listenEventLoop)),
     _accept_callback(std::bind(&TcpServer::defaultAcceptCallBack, this, std::placeholders::_1)),
     _connect_callback(std::bind(&TcpServer::defaultConnectCallBack, this, std::placeholders::_1)),
     _message_callback(std::bind(&TcpServer::defaultMessageCallBack, this, std::placeholders::_1)),
     _complete_callback(std::bind(&TcpServer::defaultCompleteCallBack, this, std::placeholders::_1)),
     _close_callback(std::bind(&TcpServer::defaultCloseCallBack, this, std::placeholders::_1))
{
    assert(m_listenEventLoop);
    assert(m_Uptr_ioEventLoops.get());
}

TcpServer::~TcpServer(){
    // TcpServer析构
    // 释放哪些资源

    if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " TcpServer deconstruct!";
}

void TcpServer::start(){

    m_Uptr_Acceptor->setAcceptCALLBACK(std::move(_accept_callback));
    m_Uptr_ioEventLoops->start();
    m_Uptr_Acceptor->start();

}



// 默认函数

void TcpServer::defaultAcceptCallBack(std::size_t fd){

    EventLoop* ioLoop = m_Uptr_ioEventLoops->getNextLoop();
    SptrTcpConn sptr_TcpConn = std::make_shared<TcpConn>(ioLoop, fd);

    sptr_TcpConn->setConnectCallBack(std::move(this->_connect_callback));
    sptr_TcpConn->setMessageCallBack(std::move(this->_message_callback));
    sptr_TcpConn->setCompleteCallBack(std::move(this->_complete_callback));
    sptr_TcpConn->setCloseCallBack(std::move(this->_close_callback));

    sptr_TcpConn->establish(); 
    this->addSptrTcpConn(sptr_TcpConn); // TcpServer管理TcpConn的生命周期

}

void TcpServer::defaultConnectCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " OnConnection with fd=[" << _ref_sptr_conn->getFD() << "]";
}

void TcpServer::defaultMessageCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " OnMessage with fd=[" << _ref_sptr_conn->getFD() << "]";
    _ref_sptr_conn->getBufferIn_all();
}

void TcpServer::defaultCompleteCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){     
    LOG_INFO << TimeStamp::now().whenCreate_str() << " OnComplete with fd=[" << _ref_sptr_conn->getFD() << "]";
}

void TcpServer::defaultCloseCallBack(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    LOG_INFO << TimeStamp::now().whenCreate_str() << " OnClose with fd=[" << _ref_sptr_conn->getFD() << "]";
    this->eraseSptrTcpConn(_ref_sptr_conn);
}