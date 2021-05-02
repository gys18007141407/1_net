#include "Logging.h"

extern Logging::LogLevel g_logLevel;

void Logging::setLogLevel(Logging::LogLevel level){
    g_logLevel = level;
}

Logging::LogLevel Logging::logLevel(){
    return g_logLevel;
}







void defaultOutFunc(const char* msg, std::size_t len){
    fwrite(msg, 1, len, stdout);
}
void defaultFlushFunc(){
    fflush(stdout);
}

Logging::outFunc g_outFunc = defaultOutFunc;
Logging::flushFunc g_flushFunc = defaultFlushFunc;

void Logging::setOutFunc(outFunc func){
    g_outFunc = func;
}

void Logging::setFlushFunc(flushFunc func){
    g_flushFunc = func;
}




const char strLevel[6][10] = {
    "[TRACE]",
    "[DEBUG]",
    "[INFO]",
    "[WARN]",
    "[ERROR]",
    "[FATAL]"
};

Logging::Logging(SourceFile file, std::size_t line): _impl(Logging::LogLevel::INFO, 0, file, line){
    _impl._stream << "[INFO] ";
}

Logging::Logging(SourceFile file, std::size_t line, Logging::LogLevel level):_impl(level, 0, file, line){
    _impl._stream << strLevel[level] << ' ';
}

Logging::Logging(SourceFile file, std::size_t line,  Logging::LogLevel level, const char* func):_impl(level, 0, file, line){
    _impl._stream << strLevel[level] << ' ' << func << ' ';
}

Logging::Logging(SourceFile file, std::size_t line, bool abort):_impl(abort?FATAL:ERROR, errno, file, line){
    _impl._stream << strLevel[abort?FATAL:ERROR] << ' ';
}

Logging::~Logging(){

    _impl.finish();

    const LogStream::Buffer& buffer = this->stream().buffer();

    g_outFunc(buffer.data(), buffer.size());

    if(_impl._level == FATAL){
        flushFunc();
        abort();
    }

}


inline LogStream& operator<<(LogStream& stream, Logging::SourceFile file){
    stream.append(file._data, file._len);
    return stream;
}


Logging::Impl::Impl(LogLevel level, int old_errno, SourceFile file, std::size_t line)
    :_level(level),
     _file(file),
     _line(line)
{

}

void Logging::Impl::finish(){
    _stream << "  ##" << _file._data << ':' << _line << '\n';
}



__thread char t_errorBUffer[512];

const char* strerror_tl(int errno_saved){
    return strerror_r(errno_saved, t_errorBUffer, sizeof(t_errorBUffer));
}
