
#ifndef FIXEDBUFFER_H
#define FIXEDBUFFER_H

#include "nonCopyable.h"
#include <cstddef>
#include <cstring>
#include <string>


class StringPiece{
public:
    StringPiece():_ptr(nullptr), _len(0){
    }

    StringPiece(const char* v): _ptr(v), _len(strlen(v)){
    }

    StringPiece(const char* v, std::size_t n): _ptr(v), _len(n){
    }

    StringPiece(const unsigned char* v):_ptr( reinterpret_cast<const char*>(v)), _len(strlen(_ptr)){
    }

    StringPiece(const unsigned char* v, std::size_t n):_ptr( reinterpret_cast<const char*>(v)), _len(n){
    }

    StringPiece(const std::string v):_ptr(v.c_str()), _len(v.size()){
    }

    const char* data() const{
        return _ptr;
    }

    std::size_t size() const{
        return _len;
    }


private:
    const char* _ptr;
    std::size_t _len;
};


#ifndef KBUFFERSIZE
#define KBUFFERSIZE
    const std::size_t kSmallSize = 4000;
    const std::size_t kBigSize = kSmallSize* 1000;
#endif

template<std::size_t SIZE>
class FixedBuffer:public NonCopyable{
public:

    FixedBuffer():_cur(_data){
        setCookie(cookieStart);
        bzero();
    }

    ~FixedBuffer(){
        setCookie(cookieEnd);
    }

    void setCookie(void(*cookie)()){
        _cookie = cookie;
    }


    const char* data() const{
        return _data;
    }

    const char* current() const {
        return _cur;
    }

    bool append(const char* from, std::size_t len){
        if(!from || len == 0){
            return append("(null)", 6);
        }
        if( available() > len){
            memcpy(_cur, from, len);
            _cur += len;
            return true;
        }
        return false;
    }

    std::size_t append_t(const char* from, std::size_t len){
        if(!from || len == 0) return 0;
        if( available() < len) len = available();
        
        memcpy(_cur, from, len);
        _cur += len;

        return len;
    }

    std::size_t append_t(const std::string s){
        return append_t(s.c_str(), s.size());
    }

    std::size_t size() const{
        return static_cast<std::size_t>(_cur-_data);
    }
    std::size_t available() const{
        return static_cast<std::size_t>(end()-_cur);
    }

    void offset(ssize_t add){
        _cur += add;
    }

    std::size_t retrive(std::size_t len){
        std::size_t rv = len <= size() ? len : size();
        _cur -= rv;
        memcpy(_data, _data+rv, size());
        return rv;
    }

    std::string retrive_get(std::size_t len){
        std::size_t rv = len <= size() ? len : size();
        std::string get(_data, rv);
        _cur -= rv;
        memcpy(_data, _data+rv, size());
        return get;
    }
    
    void reset(){
        _cur = _data;
    }

    void bzero(){
        memset(_data, 0, SIZE+8);
    }

    const char* debugString(){
        *_cur = '\0';
        return _data;
    }

    std::string toString() const{
        return std::string(_data, size());
    }

    StringPiece toStringPiece() const{
        return StringPiece(_data, size());
    }


private:

    const char* end() const{
        return _data+SIZE;
    }

    static void cookieStart();
    static void cookieEnd();

    char _data[SIZE+8];
    char* _cur;
    void (*_cookie)();
};


template<std::size_t N>
void FixedBuffer<N>::cookieStart(){

}

template<std::size_t N>
void FixedBuffer<N>::cookieEnd(){
    
}

#endif