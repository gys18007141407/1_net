
#ifndef TCPCONN_H
#define TCPCONN_H

#include "Log/FixedBuffer.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Define.h"
#include <atomic>

#ifdef DETAIL_LOG
    #define tag (1)
#else
    #define tag (0)
#endif



class IServer;

class TcpConn{

public:

    typedef std::function<void(void)> CALLBACK;

    explicit TcpConn(EventLoop* loop, std::size_t fd);
    ~TcpConn();

    std::string getBufferIn();

    std::string getBufferOut();

    std::string getBufferIn(std::size_t len);

    std::string getBufferOut(std::size_t len);

    std::size_t BufferInSize(){ return _in.size(); }
    std::size_t BufferOutSize() { return _out.size(); }

    void setServer(IServer* server){ _server = server; }

    void send(std::string msg);

    void sendBroadCast(std::string msg);

    void runAt(TimeStamp when, CALLBACK&& callback) {
        _loop->runAt(when, std::move(callback));
    }

    void runAfter(std::size_t delay, CALLBACK&& callback) {
        _loop->runAfter(delay, std::move(callback));
    }

    void runEvery(std::size_t interval, CALLBACK&& callback, std::size_t count = -1) {
        _loop->runEvery(interval, std::move(callback), count);
    }

private:

    void readCallBack();

    void writeCallBack();

    void sendInLoop(std::string msg);

#ifdef BUFFERSIZE
        FixedBuffer<BUFFERSIZE> _in;
        FixedBuffer<BUFFERSIZE> _out;
#else
    #ifndef USEBIGBUFFER
        FixedBuffer<kSmallSize> _in;
        FixedBuffer<kSmallSize> _out;
    #else
        FixedBuffer<kBigSize> _in;
        FixedBuffer<kBigSize> _out;
    #endif
#endif

    EventLoop* _loop;
    std::size_t _fd;
    Channel* _channel;

    IServer* _server;

    std::atomic<bool> _isChannelClosed;

    static std::list<TcpConn*> _conns;

    static std::mutex _mt_conns;


};

#endif