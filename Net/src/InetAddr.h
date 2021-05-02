
#ifndef INETADDR_H
#define INETADDR_H

#include <cstddef>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>


class InetAddr{
public:

    explicit InetAddr();
    explicit InetAddr(const char* IP, const std::size_t port);
    explicit InetAddr(const std::string IP, const std::size_t port);

    void setIP(const char* IP){
        _IP = std::string(IP, strlen(IP));
    }

    void setIP(std::string IP){
        _IP = IP;
    }

    void setPort(std::size_t port){
        _port = port;
        _htonsPort = htons(_port);
    }

    std::string getIP(){ return _IP; }

    std::size_t getPort() { return _port; }

    std::size_t getHtonsPort() { return _htonsPort; }

    sockaddr_in getSockAddr();

private:

    sockaddr_in _addr;

    std::size_t _port;

    std::size_t _htonsPort;

    std::string _IP;
};

#endif