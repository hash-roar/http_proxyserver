#ifndef LOG_H
#define LOG_H

#include "tools/tool.h"
using str = std::string;
using mapType = std::map<std::string, std::string>;


enum LOG_LEVEL
{
    INFO = 0,
    DEBUG,
    WARNING,
    ERROR,
    FATAL
};

class logEvent
{
public:
    str log_meaasge;
    str file_name;
    str function_name;
    int line_num;
    int thread_id;
    int process_id;
    str time;
    LOG_LEVEL level;

public:
    logEvent(const str &logMeaasge, LOG_LEVEL log_level);
    logEvent(const str &logMeaasge, LOG_LEVEL log_level, int threadId, int processId);
    logEvent(const str &logMeaasge, LOG_LEVEL log_level, int threadId, int processId, str file_name, str functionName, int lineNum);
    ~logEvent();
};

//--------------------------------------------------------------
class logAppender
{
private:
    /* data */
    pthread_mutex_t file_mutex;
    std::fstream file_stream;

public:
    logAppender(const str &file_name);
    ~logAppender();
    void write(str &mesage);
};


//-----------------------------------------------------------------
// class Log
// {
// private:
//     std::map<LOG_LEVEL, str> loglevel_map{{INFO, "INFO"}, {DEBUG, "DEBUG"}, {WARNING, "WARNING"}, {ERROR, "ERROR"}, {FATAL, "FATAL"}};
//     // std::list<logAppender> appender_list;
//     logAppender *log_appender;
// public:
//     Log(const str & log_file_name);
//     ~Log();
//     void format_log(logEvent& log_event,LOG_LEVEL log_level,str &log_str);
//     void write_log(logEvent &log_event, LOG_LEVEL log_level);

// };

// Log::Log(const str & log_file_name )
// {
//     log_appender = new logAppender(log_file_name);
// }

// Log::~Log()
// {
//     delete log_appender;
// }

class Logger
{
private:
    std::map<LOG_LEVEL, str> loglevel_map{{INFO, "INFO"}, {DEBUG, "DEBUG"}, {WARNING, "WARNING"}, {ERROR, "ERROR"}, {FATAL, "FATAL"}};
    // std::list<logAppender> appender_list;
    logAppender *log_appender;

public:
    Logger(const str &log_file_name);
    ~Logger();
    void format_log(logEvent &log_event, LOG_LEVEL log_level, str &log_str);
    void write_log(logEvent &log_event, LOG_LEVEL log_level);
};

//接口函数,方便函数调用

void write_log(Logger & log_obj,str message, LOG_LEVEL log_level);

#endif