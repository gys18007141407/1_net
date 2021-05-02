

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <cstddef>
#include <string>
#include <sys/time.h>

class TimeStamp{
public:
    static const std::size_t kMiliSec = 1000;
    static const std::size_t kMicroSec = 1000*1000;

    explicit TimeStamp(std::size_t t = 0) : _createTime(t){

    }

    static TimeStamp now();

    static TimeStamp nowAfter(std::size_t microSecDelay);

    static std::size_t now_t();

    std::size_t whenCreate() const{
        return _createTime;
    }

    std::string whenCreate_str() const{
        char t[32];

        std::size_t s = _createTime / kMicroSec;
        std::size_t microS = _createTime % kMicroSec;
        std::size_t ms = microS / kMiliSec;
        microS %= kMiliSec;

        int n = snprintf(t, 32, "%ld.%03ld.%03ld", s, ms, microS);
        return std::string(t, n);
    }

    bool isValid(){ return _createTime > 0; }

private:

    std::size_t _createTime;


};

bool operator<(const TimeStamp&x, const TimeStamp&y);
bool operator>(const TimeStamp&x, const TimeStamp&y);
bool operator==(const TimeStamp&x, const TimeStamp&y);
bool operator!=(const TimeStamp&x, const TimeStamp&y);
bool operator<=(const TimeStamp&x, const TimeStamp&y);
bool operator>=(const TimeStamp&x, const TimeStamp&y);

#endif