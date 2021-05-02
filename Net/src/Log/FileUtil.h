#ifndef FILEUTIL_H
#define FILEUTIL_H

#include "nonCopyable.h"
#include <string>
class AppendFile: public NonCopyable{
public:
    explicit AppendFile(const char* filename);
    explicit AppendFile(std::string filename);

    ~AppendFile();

    void flush();

    bool append(const char* log, std::size_t len);

    std::size_t writtenBytes() const{
        return _writtenBytes;
    }

private:
    FILE* _fp;

    std::size_t _writtenBytes;

    char buffer[1024*64];

    std::size_t write(const char* log, std::size_t len);
};



#endif