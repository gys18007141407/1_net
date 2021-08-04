/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:54:55
 */

#include <algorithm>
#include <sstream>
#include "LogStream.h"

const char digitHEX[17] = "0123456789ABCDEF";
const char digit[20] = "9876543210123456789";
const char* const zero = digit+9;

template<typename T>
std::size_t convert(const char* from, T v){
    char* p = const_cast<char*>(from), *pre = p;
    T val = v;

    do{
        int pos = static_cast<int>(val % 10);

        val /= 10;

        *p ++ = zero[pos];

    }while(val);

    if(val < 0) *p++ = '-';
    std::reverse(pre, p);

    return static_cast<std::size_t>(p-pre);
    
}

std::size_t convertHEX(const char* from, std::size_t v){

    char* p = const_cast<char*>(from), *pre = p;

    std::size_t val = v;

    do{

        int pos = static_cast<int>(val % 16);

        val /= 16;

        *p ++ = digitHEX[pos];

    }while(val);

    std::reverse(pre, p);

    return static_cast<std::size_t>(p-pre);

}

template<typename T>
void LogStream::formatInteger(T v){
    if(_buffer.available() > kMaxDecLength){
        
        _buffer.offset(convert(_buffer.current(), v));

    }
}

LogStream& LogStream::operator<<(short v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v){
    formatInteger(v);
    return *this; 
}

LogStream& LogStream::operator<<(int v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v){
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v){

    if(_buffer.available() > kMaxDecLength){
        _buffer.offset(snprintf(const_cast<char*>(_buffer.current()), kMaxDecLength, "0x%.12g", v));

    }
    return *this;
}

LogStream& LogStream::operator<<(const void* v){
    if(_buffer.available() > kMaxDecLength){
        
        _buffer.offset(convertHEX(_buffer.current(), reinterpret_cast<std::size_t>(v)));

    }
    return *this;
}


LogStream& LogStream::operator<<(const std::thread::id& id){
    std::stringstream ss;
    ss << id;
    return operator<<(ss.str());
}


