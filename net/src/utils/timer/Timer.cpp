/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-04-28 19:57:13
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:45:21
 */

#include "Timer.h"
#include <iostream>
Timer::Timer(TimeStamp timeStamp, std::size_t interval, TIMER_CALLBACK&& callback, std::size_t count)
    :_timeStamp(timeStamp),
     _interval(interval),
     _callback(callback),
     _count(count)
{
}

void Timer::moveInterval(){
    if(_count != -1 && --_count == 0) _interval = 0;
    _timeStamp = TimeStamp::nowAfter(_interval);
}