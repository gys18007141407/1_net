/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 19:57:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:45:09
 */


#include "TimeStamp.h"

std::size_t TimeStamp::now_t(){

    timeval tv;
    gettimeofday(&tv, nullptr);
    
    return tv.tv_sec*kMicroSec + tv.tv_usec;

}

TimeStamp TimeStamp::nowAfter(std::size_t microSecDelay){
    return TimeStamp(now_t()+microSecDelay);
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