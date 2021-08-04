# 1_net

A latestest version of my muduo-study repository , all codes are coded by myself, if there is a bug in this repository please tell me

<!--
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 19:37:24
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-03 21:21:21
-->
## 参考muduo设计的网络库

### 使用方法（参考example下的代码）

#### Server

##### 方法一：一个简单的用法是直接定义TcpServer,然后绑定它的回调函数
##### 方法二：对TcpServer封装一下，操作类似方法一

只需要利用TcpServer即可完成想要的效果
1、设置OnConnect回调
2、设置OnMessage回调（必需 )
3、设置OnComplete回调
......
自定义参数可放在Config.h中

```cpp
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

```

---

#### Client（基本上和Server一样）

```cpp
#include "TcpClient.h"
#include "Logging.h"

Logging::LogLevel g_logLevel = Logging::LogLevel::WARN;



void OnMessage(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    std::cout << "recv msg: " << _ref_sptr_conn->getBufferIn_all() << std::endl;
    std::string msg;
    std::getline(std::cin, msg);  // 最好换成异步的。这里只是为了演示
    _ref_sptr_conn->send(msg);
}

void OnConnect(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    _ref_sptr_conn->send("hello");
}

int main(){
    

    EventLoop loop;
    TcpClient client(&loop);
    client.setOnConnectCallBack(OnConnect);
    client.setOnMessageCallBack(OnMessage);


    client.connect();   // 先绑定回调，再连接服务器

    if(client.is_connected()) loop.loop();

    return 0;

}

```

---

#### log

1、需要定义外部全局日志等级变量 g_logLevel
2、可以设置 log 的输出方式， 默认为输出到标准输出 stdout。
   使用函数 Logging::setOutFunc(outFunc)来重定向， 其中type(OutFunc) = void(\*)(const char\* _data, std::size_t _len)
```cpp
一些输出信息：
[TRACE] => function name = "Epoll" Aug  4 15:48:54 2021 create epollfd 3 by thread 140418005440832[Epoll.cpp:24]
[INFO] => New eventLoop constructed by thread_id=[140418005440832][EventLoop.cpp:51]
[INFO] => Aug  4 15:48:54 2021 thread=[140418005440832] create a io EventLoop[EventLoopThreadPool.cpp:70]
[INFO] => Aug  4 15:48:54 2021 thread=[140418005440832] create a io EventLoop[EventLoopThreadPool.cpp:70]
[TRACE] => function name = "Epoll" Aug  4 15:48:54 2021 create epollfd 7 by thread 140417989039872[Epoll.cpp:24]
[INFO] => Aug  4 15:48:54 2021 server listening..., current IP=[127.0.0.1], current port=[8888], current htonsPort=[47138][Acceptor.cpp:57]
[TRACE] => function name = "loop" Aug  4 15:48:54 2021 eventLoop start loop with thread_id=[140418005440832][EventLoop.cpp:64]
[INFO] => New eventLoop constructed by thread_id=[140417989039872][EventLoop.cpp:51]
[TRACE] => function name = "loop" Aug  4 15:48:54 2021 eventLoop start loop with thread_id=[140417989039872][EventLoop.cpp:64]
[TRACE] => function name = "Epoll" Aug  4 15:48:54 2021 create epollfd 11 by thread 140417980647168[Epoll.cpp:24]
[INFO] => New eventLoop constructed by thread_id=[140417980647168][EventLoop.cpp:51]
[TRACE] => function name = "loop" Aug  4 15:48:54 2021 eventLoop start loop with thread_id=[140417980647168][EventLoop.cpp:64]
[TRACE] => function name = "wait" Aug  4 15:48:55 2021 fd = 6 triggered 1 event by epollfd 3[Epoll.cpp:40]
[INFO] => Aug  4 15:48:55 2021 accept a new connection with fd=[14][Acceptor.cpp:72]
[TRACE] => function name = "wait" Aug  4 15:48:55 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  4 15:48:55 2021 fd=[14] EPOLLIN because of read, bytes=[5][TcpConn.cpp:72]
[TRACE] => function name = "sendInLoop" Aug  4 15:48:55 2021 fd=[14] sendInLoop, msg length=[5][TcpConn.cpp:162]
[TRACE] => function name = "wait" Aug  4 15:48:55 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:56 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:56 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  4 15:48:56 2021 fd=[14] EPOLLIN because of read, bytes=[2][TcpConn.cpp:72]
[TRACE] => function name = "sendInLoop" Aug  4 15:48:56 2021 fd=[14] sendInLoop, msg length=[2][TcpConn.cpp:162]
[TRACE] => function name = "wait" Aug  4 15:48:56 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:57 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:57 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:57 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:58 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:58 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:48:59 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  4 15:48:59 2021 fd=[14] EPOLLIN because of read, bytes=[2][TcpConn.cpp:72]
[TRACE] => function name = "sendInLoop" Aug  4 15:48:59 2021 fd=[14] sendInLoop, msg length=[2][TcpConn.cpp:162]
[TRACE] => function name = "wait" Aug  4 15:48:59 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:00 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:01 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:01 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:01 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  4 15:49:01 2021 fd=[14] EPOLLIN because of read, bytes=[2][TcpConn.cpp:72]
[TRACE] => function name = "sendInLoop" Aug  4 15:49:01 2021 fd=[14] sendInLoop, msg length=[2][TcpConn.cpp:162]
[TRACE] => function name = "wait" Aug  4 15:49:01 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:02 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:03 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "readCallBack" Aug  4 15:49:03 2021 fd=[14] EPOLLIN because of read, bytes=[2][TcpConn.cpp:72]
[TRACE] => function name = "sendInLoop" Aug  4 15:49:03 2021 fd=[14] sendInLoop, msg length=[2][TcpConn.cpp:162]
[TRACE] => function name = "wait" Aug  4 15:49:03 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:03 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:03 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:04 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:05 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:05 2021 fd = 9 triggered 1 event by epollfd 7[Epoll.cpp:40]
[TRACE] => function name = "wait" Aug  4 15:49:07 2021 fd = 14 triggered 1 event by epollfd 7[Epoll.cpp:40]
[INFO] => Aug  4 15:49:07 2021 fd=[14] EPOLLIN but disconnect, remove the fd![TcpConn.cpp:64]
```

