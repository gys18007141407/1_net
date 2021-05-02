
#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <sys/types.h>
#include <cstddef>
#include <functional>
#include "InetAddr.h"
#include "Define.h"

#ifndef LISTENQUEUE
    #define LISTENQUEUE (0)
#endif


class Channel;
class EventLoop;

class Acceptor{

public:
    typedef std::function<void(std::size_t)> ACCEPT_CALLBACK;

    explicit Acceptor(EventLoop* loop);
    ~Acceptor();

    
    void setIP(const char* IP){ _addr.setIP(IP); }

    void setIP(std::string IP) { _addr.setIP(IP); }

    void setPort(std::size_t port){ _addr.setPort(port); }

    void setFdReuse(std::size_t fd);



    void start();

    void setAcceptCALLBACK(ACCEPT_CALLBACK&& callback){ _accept_callback = callback; }


private:

    bool init();

    sockaddr_in _sockaddr;

    InetAddr _addr;

    std::size_t _listenfd;

    Channel* _listenChannel;

    EventLoop* _listenLoop;

    ACCEPT_CALLBACK _accept_callback;
};

#endif