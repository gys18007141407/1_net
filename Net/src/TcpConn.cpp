
#include "TcpConn.h"
#include "Log/Logging.h"
#include "Timer/TimeStamp.h"
#include "IServer.h"
#include <unistd.h>
#include <sys/epoll.h>

std::list<TcpConn*> TcpConn::_conns;
std::mutex TcpConn::_mt_conns;

TcpConn::TcpConn(EventLoop* loop, std::size_t fd)
    :_loop(loop),
     _fd(fd)
{
    _channel = new Channel(_loop, _fd);

    _channel->setReadCALLBACK(std::bind(&TcpConn::readCallBack, this));

    _channel->setWriteCALLBACK(std::bind(&TcpConn::writeCallBack, this));

    _channel->setWatchEvent(EPOLLIN);

    _channel->addToLoop();

    _isChannelClosed = false;

    std::lock_guard<std::mutex> g_lock(_mt_conns);

    _conns.push_back(this);

}

TcpConn::~TcpConn(){
    if(_channel && !_isChannelClosed) delete _channel;

    std::lock_guard<std::mutex> g_lock(_mt_conns);

    _conns.remove(this);
}

void TcpConn::readCallBack(){
    
    if(_isChannelClosed){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " TcpConn readCallBack, but current fd " << _fd << " has closed!";
        return;
    }

    std::size_t restBufferSize = _in.available();
    char* temp = new char[restBufferSize+5];
    std::size_t readBytes = read(_fd, temp, restBufferSize-1);

    if(readBytes < 0 && errno == EAGAIN){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " EPOLLIN, but read error EAGIN";

    }else if(readBytes <= 0){

        LOG_INFO << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " EPOLLIN, disconnect, remove the fd by EventLoop";
        _loop->removeChannel(_channel);

        _isChannelClosed = true;

        _server->OnClose(this);

        delete _channel;
    }else{

        if(tag) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " EPOLLIN, read " << readBytes;
        temp[readBytes] = '\0';
        _in.append(temp, readBytes);

        _server->OnMessage(this);

    }

    
    delete[] temp;
}

void TcpConn::writeCallBack(){

    if(_isChannelClosed){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " TcpConn writeCallBack, but current fd " << _fd << " has closed!";
        return;
    }

    std::size_t writeBytes = write(_fd, _out.data(), _out.size());

    if(writeBytes < 0 && errno == EINTR){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " EPOLLOUT, but write error EINTR";

    }else if(writeBytes <= 0){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " EPOLLOUT, disconnect, remove the fd by EventLoop";
        _loop->removeChannel(_channel);

        _isChannelClosed = true;

        _server->OnClose(this);

        delete _channel;
    }else{

        if(_out.size() == writeBytes){
            _out.reset();
            _channel->removeWatchEvent(EPOLLOUT);

            _loop->queueInLoop(
                [this]()->void{
                    this->_server->OnComplete(this);  // if runInLoop then ; assume server send 1 byte to the client, so send call runInloop again, then call OnComplele again.....
                }
            );
        }else _out.offset(-1*writeBytes);
    }
}



std::string TcpConn::getBufferIn(){
    return _in.retrive_get(_in.size());
}

std::string TcpConn::getBufferOut(){
    return _out.retrive_get(_out.size());
}

std::string TcpConn::getBufferIn(std::size_t len){
    if(len > _in.size()) len = _in.size();

    return _in.retrive_get(len);
}

std::string TcpConn::getBufferOut(std::size_t len){
    if(len > _out.size()) len = _out.size();

    return _out.retrive_get(len);
}


void TcpConn::send(std::string msg){

    if(_isChannelClosed) LOG_WARN <<  TimeStamp::now().whenCreate_str() << " fd = " << _fd << " send msg, but Channel has closed!";

    if(std::this_thread::get_id() == _loop->getTid()) sendInLoop(msg);
    else{
        if(tag) LOG_TRACE <<std::this_thread::get_id() << " enter " << _loop->getTid() << " loop";
        _loop->queueInLoop(std::bind(&TcpConn::sendInLoop, this, msg));
    }

}

void TcpConn::sendInLoop(std::string msg){

    if(tag) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " sendInLoop, msg length = " << msg.size();

    if(_out.size()){
        std::size_t sendBytes = _out.append_t(msg.c_str(), msg.size());

        if(sendBytes < msg.size())
            LOG_WARN <<  TimeStamp::now().whenCreate_str() << " fd = " << _fd << " send msg length = " << msg.size()
                << ", but buffer only " << sendBytes << " available";
    }else{

        std::size_t sendBytes = write(_fd, msg.c_str(), msg.size());

        if(sendBytes < 0 && errno == EINTR){

            LOG_WARN << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " sendInLoop, but write error EINTR";

        }else if(sendBytes <= 0){

            LOG_WARN << TimeStamp::now().whenCreate_str() << " fd = " << _fd << " sendInLoop, disconnect, remove the fd by EventLoop";

            _loop->removeChannel(_channel);

            _isChannelClosed = true;

            _server->OnClose(this);

            delete _channel;
        }else{

            if(sendBytes < msg.size()){

                std::size_t appendBytes = _out.append_t(msg.substr(sendBytes));

                LOG_WARN <<  TimeStamp::now().whenCreate_str() << " fd = " << _fd << " sendInLoop msg length = " << msg.size()
                << ", but write only " << sendBytes << ", other " << appendBytes << " bytes will put in out_buffer";

                _channel->addWatchEvent(EPOLLOUT);

            }else{

                _loop->queueInLoop(
                    [this]()->void{
                        this->_server->OnComplete(this);
                    }
                );

            }
        }
    }

}


void TcpConn::sendBroadCast(std::string msg){

    std::lock_guard<std::mutex> g_lock(_mt_conns);

    for(auto& tcpConn : TcpConn::_conns){

        if(tcpConn != this) tcpConn->send(msg);

    }
}
