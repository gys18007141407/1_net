
#ifndef LOGDEFINE_H
#define LOGDEFINE_H

#define LOG_TRACE if(Logging::LogLevel::TRACE >= Logging::logLevel()) \
                        Logging(__FILE__, __LINE__, Logging::LogLevel::TRACE, __func__).stream()
#define LOG_DEBUG if(Logging::LogLevel::DEBUG >= Logging::logLevel()) \
                        Logging(__FILE__, __LINE__, Logging::LogLevel::DEBUG, __func__).stream()
#define LOG_INFO if(Logging::LogLevel::INFO >= Logging::logLevel())   \
                        Logging(__FILE__, __LINE__, Logging::LogLevel::INFO).stream()

#define LOG_WARN Logging(__FILE__, __LINE__, Logging::LogLevel::WARN).stream()
#define LOG_ERROR Logging(__FILE__, __LINE__, Logging::LogLevel::ERROR).stream()
#define LOG_FATAL Logging(__FILE__, __LINE__, Logging::LogLevel::FATAL).stream()

#define LOG_SYSERROR Logging(__FILE__, __LINE__, false);
#define LOG_SYSFATAL Logging(__FILE__, __LINE__, true);


#endif