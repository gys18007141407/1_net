
#include "TimeStamp.h"

std::size_t TimeStamp::now_t(){

    timeval tv;
    gettimeofday(&tv, nullptr);
    
    return tv.tv_sec*kMicroSec + tv.tv_usec;

}

TimeStamp TimeStamp::nowAfter(std::size_t delay){
    return TimeStamp(now_t()+delay);
}

TimeStamp TimeStamp::now(){
    return TimeStamp(TimeStamp::now_t());
}


bool operator<(const TimeStamp&x, const TimeStamp&y){
    return x.whenCreate() < y.whenCreate();
}

bool operator>(const TimeStamp&x, const TimeStamp&y){
    return x.whenCreate() > y.whenCreate();
}
bool operator==(const TimeStamp&x, const TimeStamp&y){
    return x.whenCreate() == y.whenCreate();
}
bool operator!=(const TimeStamp&x, const TimeStamp&y){
    return x.whenCreate() != y.whenCreate();
}
bool operator<=(const TimeStamp&x, const TimeStamp&y){
    return x.whenCreate() <= y.whenCreate();
}
bool operator>=(const TimeStamp&x, const TimeStamp&y){
    return x.whenCreate() >= y.whenCreate();
}