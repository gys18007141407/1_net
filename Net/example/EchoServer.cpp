
#include "../src/IServer.h"
#include "../src/Log/Logging.h"
#include "../src/Log/AsynLog.h"

#include <iostream>


class EchoServer:public IServer{

public:

    explicit EchoServer(){
        _server = nullptr;
    }

    ~EchoServer(){
        delete _server;
    }

    void OnConnect(TcpConn* tcpConn){
        std::cout << "OnConnect invoked! do nothing ... " << std::endl;
    }

    void OnMessage(TcpConn* tcpConn){
        std::cout << "OnMessage invoked! " << std::endl;

        tcpConn->send(tcpConn->getBufferIn());
        //tcpConn->sendBroadCast(tcpConn->getBufferIn());  // send to all client connected with this server

        tcpConn->runEvery(300, []()->void{
            std::cout << std::this_thread::get_id() << " hello world" << std::endl;
        }, 3);
    }

    void OnComplete(TcpConn* tcpConn){
        std::cout << "OnComplete invoked! do nothing ... " << std::endl;
    }

    void OnClose(TcpConn* tcpConn){
        std::cout << "OnClose invoked! do nothing ... " << std::endl;
    }

    void run(){
        _server = new TcpServer(3);
        _server->setServer(this);

        _server->start();
    }


private:

    TcpServer* _server;


};


AsynLog Log(__FILE__, 1024*1024 , 3);
Logging::LogLevel g_logLevel = Logging::LogLevel::TRACE;

void outFunc(const char* msg, std::size_t len){
    Log.append(msg, len);
}


int main(){
    

    Logging::setOutFunc(outFunc);
    
    Log.start();

    EchoServer server;
    server.run();

    return 0;

}