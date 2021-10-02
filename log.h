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
logEvent::logEvent(const str &logMeaasge, LOG_LEVEL log_level)
{
    this->log_meaasge = logMeaasge;
    this->level = log_level;
    this->time = get_local_time();
}
logEvent::logEvent(const str &logMeaasge, LOG_LEVEL log_level, int threadId, int processId)
{
    this->log_meaasge = logMeaasge;
    this->level = log_level;
    this->process_id = processId;
    this->thread_id = threadId;
    this->time = get_local_time();
}
logEvent::logEvent(const str &logMeaasge, LOG_LEVEL log_level, int threadId, int processId, str fileName, str functionName, int lineNum)
{
    this->log_meaasge = logMeaasge;
    this->level = log_level;
    this->process_id = processId;
    this->thread_id = threadId;
    this->file_name = fileName;
    this->function_name = functionName;
    this->time = get_local_time();
}

logEvent::~logEvent()
{
}

class log
{
private:
    std::map<LOG_LEVEL, str> loglevel_map{{INFO, "INFO"}, {DEBUG, "DEBUG"}, {WARNING, "WARNING"}, {ERROR, "ERROR"}, {FATAL, "FATAL"}};
    // std::list<logAppender> appender_list;
    logAppender log_appender;
public:
    log(const str & log_file_name);
    void log::format_log(logEvent& log_event,LOG_LEVEL log_level,str &log_str);
    void write_log();
    ~log();
};

log::log(const str & log_file_name )
{
    
}

log::~log()
{
}

class logAppender
{
private:
    /* data */
    pthread_mutex_t file_mutex;
    std::fstream file_stream;
public:
    logAppender(const str & file_name);
    ~logAppender();
    void write(str &mesage);
};

logAppender::logAppender(const str & file_name)
{
    file_stream.open(file_name,std::ios::app|std::ios::out);
}

logAppender::~logAppender()
{
    file_stream.close();
}

#endif