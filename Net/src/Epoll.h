
#ifndef EPOLL_H
#define EPOLL_H

#include <sys/epoll.h>
#include <vector>

#include "Channel.h"
#include "Define.h"

#define MAXEVENTS (200)


#ifndef TIMEOUT

    #define TIMEOUT (1000*60)

#endif

#ifdef DETAIL_LOG
    #define tag (1)
#else
    #define tag (0)
#endif


#define DETAIL_LOG


class Epoll{
public:
    
    explicit Epoll();
    ~Epoll();

    void update(Channel* channel);

    void add(Channel* channel);

    void remove(Channel* channel);

    std::size_t wait(std::vector<Channel*>& channels);

private:
    epoll_event _epollEvents[MAXEVENTS];

    epoll_event _ev;

    const std::size_t _epollfd;

};

#endif