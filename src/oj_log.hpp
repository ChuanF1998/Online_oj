#pragma once

#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <cstdio>


//当前实现的log服务也是在控制台进行输出
//格式
//[时间 日志等级 文件：行号] 具体的日志信息

class LogTime
{
    public:
        static int64_t GetTimeStamp()
        {
            struct timeval tv;
            gettimeofday(&tv, NULL);
            return tv.tv_sec;
        }

        //返回 年-月-日-时：分：秒
        static void GetTimeStamp(std::string* timestamp)
        {
            time_t sys_time;
            time(&sys_time);
            struct tm* st = localtime(&sys_time);
            char buf[30] = {'\0'};
            snprintf(buf, sizeof(buf) - 1, "%04d-%02d-%02d %02d:%02d:%02d", st->tm_year+ 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
            timestamp->assign(buf, strlen(buf));
        }
};


//日志等级
//INFO WARNING ERROR FATAL DEBUG
const char* Level[] = {"INFO", "WARNING", "ERROR", "FATAL", "DEBUG"};

enum LogLevel
{
    INFO = 0,
    WARNING,
    ERROR,
    FATAL,
    DEBUG
};

inline std::ostream& Log(LogLevel lev, const char* file, int line, const std::string& logmsg)
{
    std::string level_info = Level[lev];
    std::string TimeStamp;
    LogTime::GetTimeStamp(&TimeStamp);
    std::cout << "[" << TimeStamp << " " << level_info << " " << file << ":" << line << "]" << logmsg;
    return std::cout;
}


#define LOG(lev, msg) Log(lev, __FILE__, __LINE__, msg)
