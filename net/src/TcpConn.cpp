
/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-29 17:06:35
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:59:17
 */
#include <unistd.h>
#include <sys/epoll.h>

#include "TcpConn.h"
#include "Logging.h"
#include "TimeStamp.h"


TcpConn::TcpConn(EventLoop* loop, std::size_t fd, bool thread_safe)
    :_loop(loop),
     _fd(fd),
     _thread_safe(thread_safe)
{
    _in.reset(new Buffer(_thread_safe));
    _out.reset(new Buffer(_thread_safe));
}

TcpConn::~TcpConn(){
    if(_channel) delete _channel;  // channel由TcpConn管理
}

void TcpConn::establish(){
    _channel = new Channel(_loop, _fd);

    _channel->setReadCALLBACK(std::bind(&TcpConn::readCallBack, this));

    _channel->setWriteCALLBACK(std::bind(&TcpConn::writeCallBack, this));

    _channel->setWatchEvent(EPOLLIN);

    _channel->addToLoop();

    _isChannelClosed = false;

    _connect_callback(this->getSptrTcpConn());
}

void TcpConn::readCallBack(){
    
    if(_isChannelClosed){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " TcpConn readCallBack, but current fd=[" << _fd << "] has closed!";
        return;
    }

    char temp[2048];
    int32_t readBytes = read(_fd, temp, 2048);

    if(readBytes < 0 && errno == EAGAIN){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] EPOLLIN, but read error EAGIN";

    }else if(readBytes <= 0){

        LOG_INFO << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] EPOLLIN but disconnect, remove the fd!";
        _loop->removeChannel(_channel);

        _isChannelClosed = true;

        _close_callback(this->getSptrTcpConn());
    }else{

        if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] EPOLLIN because of read, bytes=[" << readBytes << "]";
        temp[readBytes] = '\0';
        _in->append(temp, readBytes);

        _message_callback(this->getSptrTcpConn());

    }
}

void TcpConn::writeCallBack(){

    if(_isChannelClosed){
        LOG_WARN << TimeStamp::now().whenCreate_str() << " TcpConn writeCallBack, but current fd=[" << _fd << "] has closed!";
        return;
    }

    int32_t writeBytes = write(_fd, _out->data(), _out->size());

    if(writeBytes < 0 && errno == EINTR){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] EPOLLOUT, but write error EINTR";

    }else if(writeBytes <= 0){

        LOG_WARN << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] EPOLLOUT because of disconnection, remove the fd";
        _loop->removeChannel(_channel);

        _isChannelClosed = true;

        _close_callback(this->getSptrTcpConn());
    }else{
        if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] EPOLLOUT because of write, bytes=[" << writeBytes << "]";
        if(_out->size() == writeBytes){
            _out.reset();
            _channel->removeWatchEvent(EPOLLOUT);

            _loop->queueInLoop(
                [this]()->void{
                    this->_complete_callback(this->getSptrTcpConn());  // if runInLoop then ; assume server send 1 byte to the client, so send call runInloop again, then call OnComplele again.....
                }
            );
        }else _out->offset(writeBytes);
    }
}



std::string TcpConn::getBufferIn_all(){
    return _in->retrive_get_all();
}

std::string TcpConn::getBufferOut_all(){
    return _out->retrive_get_all();
}

std::string TcpConn::getBufferIn(std::size_t len){
    return _in->retrive_get(len);
}

std::string TcpConn::getBufferOut(std::size_t len){
    return _out->retrive_get(len);
}

std::string TcpConn::lookBufferIn(std::size_t len){
    return _in->to_string(len);
}

std::string TcpConn::lookBufferOut(std::size_t len){
    return _out->retrive_get(len);
}


void TcpConn::send(const std::string& msg){

    if(msg.size() == 0){
        LOG_WARN <<  TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] send msg, but msg length=[0], send cancelled!";
    }else if(_isChannelClosed) LOG_WARN <<  TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] send msg, but Channel has closed!";
    else{

        if(get_thread_id() == _loop->getTid()) sendInLoop(msg);
        else{
            if(DETAIL_LOG) LOG_TRACE << get_thread_id() << " enter " << _loop->getTid() << " loop";
            _loop->queueInLoop(std::bind(&TcpConn::sendInLoop, this, msg));
        }
    }

}

void TcpConn::sendInLoop(const std::string& msg){

    if(DETAIL_LOG) LOG_TRACE << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] sendInLoop, msg length=[" << msg.size() << "]";

    if(_out->size()){
        _out->append(msg.c_str(), msg.size());
    }else{

        std::size_t sendBytes = write(_fd, msg.c_str(), msg.size());

        if(sendBytes < 0 && errno == EINTR){

            LOG_WARN << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] sendInLoop, but write error EINTR";

        }else if(sendBytes <= 0){

            LOG_WARN << TimeStamp::now().whenCreate_str() << " fd=[" << _fd << "] sendInLoop, disconnect, remove the fd";

            _loop->removeChannel(_channel);

            _isChannelClosed = true;

            _close_callback(this->getSptrTcpConn());

        }else{

            if(sendBytes < msg.size()){
                std::string send_msg = msg.substr(sendBytes);
                _out->append(send_msg.c_str(), send_msg.size());
                _channel->addWatchEvent(EPOLLOUT);

            }else{

                _loop->queueInLoop(
                    [this]()->void{
                        this->_complete_callback(this->getSptrTcpConn());
                    }
                );

            }
        }
    }

}


