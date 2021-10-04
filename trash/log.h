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
    logEvent(const str logMeaasge, LOG_LEVEL log_level);
    logEvent(const str logMeaasge, LOG_LEVEL log_level, int threadId, int processId);
    logEvent(const str logMeaasge, LOG_LEVEL log_level, int threadId, int processId, str file_name, str functionName, int lineNum);
    ~logEvent();
};

//--------------------------------------------------------------
class logAppender
{
private:
    /* data */
    pthread_mutex_t file_mutex;
    std::ofstream file_stream;

public:
    logAppender(const str &file_name);
    ~logAppender();
    void write(str &mesage);
};

//-----------------------------------------------------------------

class Logger
{
private:
    std::map<LOG_LEVEL, str> loglevel_map{{INFO, "INFO"}, {DEBUG, "DEBUG"}, {WARNING, "WARNING"}, {ERROR, "ERROR"}, {FATAL, "FATAL"}};
    // std::list<logAppender> appender_list;
    std::map<std::string, logAppender *> pLog_appander_List;
    
    Logger();
public:
    static Logger *log_obj_;
public:
    static Logger *get_log_obj()
    {
        if (log_obj_ == nullptr)
        {
            /* code */
            log_obj_ = new Logger();
        }
        return log_obj_;
    }
    void add_logappender(std::string log_path);
    void format_log(logEvent &log_event, LOG_LEVEL log_level, str &log_str);
    void write_log(logEvent &log_event, LOG_LEVEL log_level, std::string log_path);
    ~Logger();
};

//接口函数,方便函数调用

void write_log(Logger &log_obj, str message, LOG_LEVEL log_level);

#endif