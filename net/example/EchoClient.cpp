/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-30 09:54:52
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-04 13:58:15
 */

#include "TcpClient.h"
#include "Logging.h"

Logging::LogLevel g_logLevel = Logging::LogLevel::WARN;

void OnMessage(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    std::cout << "recv server msg: " << _ref_sptr_conn->getBufferIn_all() << std::endl;
}

void OnConnect(const TcpConn::SptrTcpConn& _ref_sptr_conn){
    _ref_sptr_conn->send("hello");
}

int main(){
    TcpClient::SptrTcpClient client;

    // 坑点: 如果忽略返回值，则会阻塞！！！因为async的返回值是一个future对象，在析构时要等待异步任务执行结束
    auto _ = std::async(std::launch::async, 
        [&client](){
            EventLoop loop;
            client.reset(new TcpClient(&loop));

            client->setOnConnectCallBack(OnConnect);
            client->setOnMessageCallBack(OnMessage);
            client->connect();     // 先设置回调，后建立连接

            loop.loop();
        }
    );
   
    while(!client.get() || !client->is_connected()); // 等待连接

    std::string msg;
    while(std::getline(std::cin, msg), msg != "quit"){
        if(msg.size() == 0) continue;
        client->send(msg);
    }

    return 0;

}