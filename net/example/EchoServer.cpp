/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-30 09:54:43
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 11:06:48
 */

#include "TcpServer.h"
#include "AsynLog.h"
#include "Logging.h"

#include <iostream>

class EchoServer final{

public:

    explicit EchoServer(EventLoop* _loop){
        _server = new TcpServer(_loop);
        _server->setConnectCallBack(std::bind(&EchoServer::OnConnect, this, std::placeholders::_1));
        _server->setMessageCallBack(std::bind(&EchoServer::OnMessage, this, std::placeholders::_1));
        _server->setCompleteCallBack(std::bind(&EchoServer::OnComplete, this, std::placeholders::_1));
        _server->setCloseCallBack(std::bind(&EchoServer::OnClose, this, std::placeholders::_1));     // 如果非必要则最好使用默认的函数
    }

    ~EchoServer(){
        if(_server) delete _server;
    }

    void OnConnect(const TcpConn::SptrTcpConn& _ref_sptr_conn){
        std::cout << "OnConnect invoked! do nothing ... " << std::endl;
    }

    void OnMessage(const TcpConn::SptrTcpConn& _ref_sptr_conn){
        
        std::string msg = _ref_sptr_conn->getBufferIn_all();

        // echo：send to one
        // _ref_sptr_conn->send(msg);
        
        // chat: send to all
        std::vector<TcpConn::SptrTcpConn> all_TcpConn;
        _server->getAllTcpConn(all_TcpConn);
        for(auto& sptr_tcpconn: all_TcpConn) sptr_tcpconn->send(msg);

        
        // 设置定时事件
        _ref_sptr_conn->runEvery(1000*1000, []()->void{
            std::cout << get_thread_id() << " hello world" << std::endl;
        }, 3);
    }

    void OnComplete(const TcpConn::SptrTcpConn& _ref_sptr_conn){
        std::cout << "OnComplete invoked! do nothing ... " << std::endl;
    }

    void OnClose(const TcpConn::SptrTcpConn& _ref_sptr_conn){              // 如果非必要则最好使用默认的函数
        std::cout << "OnClose invoked! rm TcpConn from TcpServer ... " << std::endl;
        _server->eraseSptrTcpConn(_ref_sptr_conn);                         // 从TcpServer中删除该TcpConn，避免占用太多空间。避免Tcperver中含有太多无效的TcpConn
    }

    void start(){
        _server->start();  // 先绑定回调再启动
    }


private:

    TcpServer* _server;

};


// 日志相关
AsynLog Log(__FILE__, 1024*1024 , 3);
Logging::LogLevel g_logLevel = Logging::LogLevel::TRACE;

void outFunc(const char* msg, std::size_t len){
    Log.append(msg, len);
}
//


int main(){

    Logging::setOutFunc(outFunc);
    Log.start();

    EventLoop loop;

    EchoServer server(&loop);  
    // TcpServer server(&loop);  // 更简单的

    server.start();  // 先绑定回调再开始
    loop.loop();

    return 0;
}