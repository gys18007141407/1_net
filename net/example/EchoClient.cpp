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
    std::cout << "recv msg: " << _ref_sptr_conn->getBufferIn_all() << std::endl;
    std::string msg;
    std::getline(std::cin, msg);
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