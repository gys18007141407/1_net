/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 20:50:56
 */

#include <assert.h>
#include "AsynLog.h"
#include "LogFile.h"

AsynLog::AsynLog(const std::string& basename, std::size_t rollSize, std::size_t flushInterval)
    :_basename(basename),
     _rollSize(rollSize),
     _flushInterval(flushInterval),
     _curBuffer(new Buffer()),
     _nextBuffer(new Buffer()),
     _running(false)

{
    _curBuffer->bzero();
    _nextBuffer->bzero();
    _pBuffers.reserve(8);
}

bool AsynLog::append(const char* log, std::size_t len)
{
    bool flag = false;
    {    
        std::lock_guard<std::mutex> g_lock(_mt);

        if(_curBuffer->available() <= len){

            _pBuffers.push_back(std::move(_curBuffer));

            if(_nextBuffer) _curBuffer = std::move(_nextBuffer);
            else _curBuffer.reset(new Buffer());
        }

        flag = _curBuffer->append(log, len);
    }
    _cond.notify_all();
    return flag;
}


void AsynLog::threadFUNC(){
    assert(_running);

    LogFile logfile(_basename, _rollSize, false, _flushInterval);

    BufferPtr pbuffer1(new Buffer());
    BufferPtr pbuffer2(new Buffer());

    BufferVector pbuffersWrite;
    pbuffersWrite.reserve(8);

    pbuffer1->bzero();
    pbuffer2->bzero();

    while(_running){
        assert(pbuffer1 && pbuffer1->size() == 0);
        assert(pbuffer2 && pbuffer2->size() == 0);
        assert(pbuffersWrite.size() == 0);

        {
            std::unique_lock<std::mutex> u_lock(_mt);

            if(_pBuffers.size() == 0){

                _cond.wait_for(u_lock, std::chrono::milliseconds(_flushInterval*1000), [this]()->bool{
                    return this->_pBuffers.size() || this->_curBuffer->size();
                });
            }


            _pBuffers.push_back(std::move(_curBuffer));
            _curBuffer = std::move(pbuffer1);
            _pBuffers.swap(pbuffersWrite);

            if(!_nextBuffer) _nextBuffer = std::move(pbuffer2);
        }

        assert(pbuffersWrite.size());

        if(pbuffersWrite.size() > 16){
            char templog[128];
            int n = snprintf(templog, 127, "Too Many Log.....Drop %ld Log Buffers\n", pbuffersWrite.size()-2);
            templog[n] = '\0';

            pbuffersWrite.resize(2);
            logfile.append(templog, n);
            fputs(templog, stderr);
        }

        for(auto& pb : pbuffersWrite){
            logfile.append(pb->data(), pb->size());
        }

        if(pbuffersWrite.size() > 2) pbuffersWrite.resize(2);

        if(!pbuffer1){
            assert(pbuffersWrite.size());

            pbuffer1 = std::move(pbuffersWrite.back());
            pbuffersWrite.pop_back();
            pbuffer1->reset();
        }

        if(!pbuffer2){
            assert(pbuffersWrite.size());

            pbuffer2 = std::move(pbuffersWrite.back());
            pbuffersWrite.pop_back();
            pbuffer2->reset();
        }

        pbuffersWrite.clear();

        logfile.flush();


    }

    logfile.flush();
}