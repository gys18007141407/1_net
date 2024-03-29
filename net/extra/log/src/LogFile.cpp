/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:00:20
 */
#include <cassert>
#include <thread>
#include "LogFile.h"

LogFile::LogFile(const std::string& basename,
                std::size_t rollSize,
                bool threadSafe,
                std::size_t flushInterval,
                std::size_t checkEveryN)
    :_basename( basename.rfind('/') == -1?basename:basename.substr(basename.rfind('/')+1)),
     _rollize(rollSize),
     _threadSafe(threadSafe),
     _flushInterval(flushInterval),
     _checkEveryN(checkEveryN),
     _count(0),
     _startOfPeriod(0),
     _lastRoll(0),
     _lastFlush(0)
{
    assert(_basename.size());
    rollFile();
}

LogFile::~LogFile(){

}

bool LogFile::append(const char* log, std::size_t len){
    if(_threadSafe){
        std::lock_guard<std::mutex> g_lock(_mt);
        return append_unlocked(log, len);
    }
    return append_unlocked(log, len);
}

bool LogFile::append_unlocked(const char* log, std::size_t len){
    bool flag = _upFile->append(log, len);

    if(_upFile->writtenBytes() > _rollize){
        rollFile();
    }else{
        
        if(++_count >= _checkEveryN){
            _count = 0;

            std::time_t now = ::time(nullptr);
            std::time_t period = now / kRollPerSeconds * kRollPerSeconds;

            if(period != _startOfPeriod){
                rollFile();
            }else if(now - _lastFlush > _flushInterval){
                _lastFlush = now;
                _upFile->flush();
            }
        }
    }
    return flag;
}

void LogFile::flush(){
    if(_threadSafe){
        std::lock_guard<std::mutex> g_lock(_mt);
        _upFile->flush();
    }
    else _upFile->flush();
}

bool LogFile::rollFile(){
    std::time_t now;
    std::string filename = getLogFileName(_basename, &now);

    std::time_t start = now / kRollPerSeconds * kRollPerSeconds;
    if(now > _lastRoll){
        _lastRoll = now;
        _lastFlush = now;
        _startOfPeriod = start;
        _upFile.reset(new AppendFile(filename));
    }

    return true;
}

std::string LogFile::getLogFileName(const std::string& basename, std::time_t* now){
    std::string filename;
    filename.reserve(basename.size()+64);
    filename = basename;

    char timebuf[32];
    int len;
    struct tm nowtime;

    *now = ::time(nullptr);
    localtime_r(now, &nowtime);
    
    strftime(timebuf, 32, ".%Y-%m-%d.%H-%M-%S.log",&nowtime);
    filename += timebuf;

    return filename;
}
