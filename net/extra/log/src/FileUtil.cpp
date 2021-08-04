/*
 * *@*	Description: 
 * *@*	Version: 
 * *@*	Author: yusheng Gao
 * *@*	Date: 2021-08-02 20:47:28
 * *@*	LastEditors: yusheng Gao
 * *@*	LastEditTime: 2021-08-02 21:00:20
 */
#include "FileUtil.h"
#include "Logging.h"

AppendFile::AppendFile(const char* filename)
    :_fp(fopen(filename, "ae")),
     _writtenBytes(0)
{

}

AppendFile::AppendFile(std::string filename)
    :_fp(fopen(filename.c_str(), "ae")),
     _writtenBytes(0)
{

}

AppendFile::~AppendFile(){
    if(_fp) fclose(_fp);
}

bool AppendFile::append(const char* log, std::size_t len){

    std::size_t curwritten = write(log, len);
    std::size_t rest = len - curwritten;

    while(rest){
        std::size_t add = write(log + curwritten, rest);

        if(curwritten == 0){
            
            int err = ferror(_fp);
            if(err){
                fprintf(stderr, "AppendFile::append() faild %s \n", strerror_tl(err));
            }
            return false;
        }

        rest -= add;
        curwritten += add;
    }
    _writtenBytes += len;
    return true;



}

void AppendFile::flush(){
    ::fflush(_fp);
}

std::size_t AppendFile::write(const char* log, std::size_t len){
    return fwrite(log, 1, len, _fp);
}
