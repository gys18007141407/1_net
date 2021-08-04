/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-03 21:36:59
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 21:37:00
 */

#include "TcpClient.h"

TcpClient::TcpClient(EventLoop* loop, bool thread_safe):
    _loop(loop),
    _is_connect(false),
    _thread_safe(thread_safe),
    _connect_callback(std::bind(&TcpClient::defaultOnConnectCallBack, this, std::placeholders::_1)),
    _message_callback(std::bind(&TcpClient::defaultOnMessageCallBack, this, std::placeholders::_1)),
    _complete_callback(std::bind(&TcpClient::defaultOnCompleteCallBack, this, std::placeholders::_1)),
    _close_callback(std::bind(&TcpClient::defaultOnCloseCallBack, this, std::placeholders::_1))
{ 
    if(-1 == (_fd = socket(AF_INET, SOCK_STREAM, 0))){
        puts("socket create error!!!");
        exit(EXIT_FAILURE);
    }
}

TcpClient::TcpClient(EventLoop* loop, const std::string& ip, std::size_t port, bool thread_safe):
    _loop(loop),
    _is_connect(false),
    _thread_safe(thread_safe),
    _connect_callback(std::bind(&TcpClient::defaultOnConnectCallBack, this, std::placeholders::_1)),
    _message_callback(std::bind(&TcpClient::defaultOnMessageCallBack, this, std::placeholders::_1)),
    _complete_callback(std::bind(&TcpClient::defaultOnCompleteCallBack, this, std::placeholders::_1)),
    _close_callback(std::bind(&TcpClient::defaultOnCloseCallBack, this, std::placeholders::_1))
{
    _server_addr.setIP(ip);
    _server_addr.setPort(port);
    if(-1 == (_fd = socket(AF_INET, SOCK_STREAM, 0))){
        puts("socket create error!!!");
        exit(EXIT_FAILURE);
    }
}

TcpClient::~TcpClient(){
    if(-1 != _fd){
        close(_fd);
    }
}

bool TcpClient::connect(){

    sockaddr_in server_addr = this->getSockAddr();
    if(-1 == ::connect(_fd, (sockaddr*)&server_addr, sizeof(sockaddr))){
        return false;
    }

    setFdReuse(_fd);

    _sptr_TcpConn.reset(new TcpConn(_loop, _fd));
    _sptr_TcpConn->setConnectCallBack(std::move(_connect_callback));
    _sptr_TcpConn->setMessageCallBack(std::move(_message_callback));
    _sptr_TcpConn->setCompleteCallBack(std::move(_complete_callback));
    _sptr_TcpConn->setCloseCallBack(std::move(_close_callback));

    _sptr_TcpConn->establish();
    _is_connect = true;

    return true;
}

void TcpClient::defaultOnConnectCallBack(const SptrTcpConn& _ref_sptr_conn){
    std::cout << TimeStamp::now().whenCreate_str() << " connect server successs!!!" << std::endl;
}
void TcpClient::defaultOnMessageCallBack(const SptrTcpConn& _ref_sptr_conn){
    std::cout << TimeStamp::now().whenCreate_str() << " recv msg=[" << _ref_sptr_conn->getBufferIn_all() << "] from server" << std::endl;
}
void TcpClient::defaultOnCompleteCallBack(const SptrTcpConn& _ref_sptr_conn){
    std::cout << TimeStamp::now().whenCreate_str() << " complete send data in buffer" << std::endl;
}
void TcpClient::defaultOnCloseCallBack(const SptrTcpConn& _ref_sptr_conn){
    std::cout << TimeStamp::now().whenCreate_str() << " server closed!!!" << std::endl;
}