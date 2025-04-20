#ifndef LOGGING_H
#define LOGGING_H

#include "../base/Timestamp.h"
#include "LogStream.h"

#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <functional>

class SourceFile
{
public:
    explicit SourceFile(const char* filename) : data_(filename)
    {
        const char* slash = strrchr(filename, '/');
        if(slash)
        {
            data_ = slash + 1;
        }
        size_ = static_cast<int>(strlen(data_));
    }

    const char* data_;
    int size_;
};

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        LEVEL_COUNT,
    };

    Logger(const char* file, int line);
    Logger(const char* file, int line, LogLevel level);
    Logger(const char* file, int line, LogLevel level, const char* func);
    ~Logger();Logginchar* msg, int len)>;
    using FlushFunc = std::function<void()>;
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl
    {
    public:
        using LogLevel = Logger::LogLevel;
        Impl(LogLevel level, int savedErrno, const char* file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };

    Impl impl_;
};


extern Logger::LogLevel g_logLevel;  //具有外部链接性，多文件共享变量

inline Logger::LogLevel logLevel()
{
    return g_logLevel;
}

const char* getErrnoMsg(int savedErrno);

#define LOG_DEBUG if (logLevel() <= Logger::DEBUG) \
    Logger(__FILE__, __LINE__, Logger::DEBUG, __func__).stream()
#define LOG_INFO if(logLevel() <= Logger::INFO) \
    Logger(__FILE__, __LINE__).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()

#endif