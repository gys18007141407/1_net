
#ifndef ISERVER_H
#define ISERVER_H

#include "TcpConn.h"
#include "TcpServer.h"


class IServer{
public:

    IServer() = default;

    virtual ~IServer() = default;

    virtual void OnConnect(TcpConn* tcpConn) = 0;

    virtual void OnMessage(TcpConn* tcpConn) = 0;

    virtual void OnComplete(TcpConn* tcpConn) = 0;

    virtual void OnClose(TcpConn* tcpConn) =  0;


};


#endif