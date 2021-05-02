
#include "Timer.h"
#include <iostream>
Timer::Timer(TimeStamp timeStamp, std::size_t interval, CALLBACK&& callback, std::size_t count)
    :_timeStamp(timeStamp),
     _interval(interval),
     _callback(callback),
     _count(count)
{

}

void Timer::moveInterval(){
    if(_count != -1 && --_count == 0) _interval = 0;
    std::cout << "_count = " << _count << std::endl;;
    _timeStamp = TimeStamp::nowAfter(_interval);
}