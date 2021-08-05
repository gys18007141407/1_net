/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 17:06:35
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:59:17
 */

#include "InetAddr.h"
#include "Logging.h"
#include "TimeStamp.h"

InetAddr::InetAddr()
    :_IP("127.0.0.1"),
     _port(8888),
     _htonsPort(htons(_port))
{
    _addr.sin_family = AF_INET;
    
}

InetAddr::InetAddr(const char* IP, const std::size_t port)
    :_IP(std::string(IP, strlen(IP))),
     _port(port),
     _htonsPort(htons(_port))
{
    _addr.sin_family = AF_INET;
}

InetAddr::InetAddr(const std::string& IP, const std::size_t port)
    :_IP(IP),
     _port(port),
     _htonsPort(htons(_port))
{
    _addr.sin_family = AF_INET;
}

sockaddr_in InetAddr::getSockAddr(){
    
    if(-1 == inet_pton(AF_INET, _IP.c_str(), &_addr.sin_addr)){
        LOG_ERROR << TimeStamp::now().whenCreate_str() << " ip assign failed with ip=[" << _IP << "], " << strerror(errno);
    }

    _addr.sin_port = _htonsPort;

    return _addr;
}