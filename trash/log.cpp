#include "./log.h"

logEvent::logEvent(const str logMeaasge, LOG_LEVEL log_level)
{
    this->log_meaasge = logMeaasge;
    this->level = log_level;
    this->time = get_local_time();
}
logEvent::logEvent(const str logMeaasge, LOG_LEVEL log_level, int threadId, int processId)
{
    this->log_meaasge = logMeaasge;
    this->level = log_level;
    this->process_id = processId;
    this->thread_id = threadId;
    this->time = get_local_time();
}
logEvent::logEvent(const str logMeaasge, LOG_LEVEL log_level, int threadId, int processId, str fileName, str functionName, int lineNum)
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
//-------------------------------------------->

logAppender::logAppender(const str &file_name)
{
    file_stream.open(file_name, std::ios::app | std::ios::out);
    if (!file_stream.is_open())
    {
        std::cout << "open log file error" << std::endl;
    }
}

logAppender::~logAppender()
{
    file_stream.close();
}
void logAppender::write(str &meaasge)
{
    pthread_mutex_lock(&file_mutex);
    file_stream << meaasge << std::endl;
    pthread_mutex_unlock(&file_mutex);
}
//---------------------------------------------->

Logger::Logger()
{
}

Logger::~Logger()
{
}

void Logger::add_logappender(std::string log_path)
{
    if (pLog_appander_List.find(log_path) == pLog_appander_List.end())
    {
        pLog_appander_List.emplace(std::make_pair(log_path, new logAppender(log_path)));
    }
}

void Logger::format_log(logEvent &log_event, LOG_LEVEL log_level, str &log_str)
{
    char buffer[512];
    const char *log_level_str = loglevel_map[log_level].c_str();
    switch (log_level)
    {
    case INFO:
        // const char *format = "[%s] [%s]";
        // sprintf(buffer, format, log_level_str, log_event.time);
        // break;
    case DEBUG:
        // const char *format1 = "[%s] [%s]";
        sprintf(buffer, "[%s] [%s]", log_level_str, log_event.time.c_str());
        break;
    case WARNING:
        // const char *format2 = "[%s] [%s] [PID:%4d] [TID:%4d]";
        sprintf(buffer, "[%s] [%s] [PID:%4d] [TID:%4d]", log_level_str, log_event.time.c_str(), log_event.process_id,
                log_event.thread_id);
        break;
    case ERROR:
        //  const char *format = "[%s] [%s] [PID:%4d] [TID:%4d] [%-s] [%s:%4d]";         // [日志级别] [时间] [进程id] [线程id] [文件名] [函数名:行号]
    case FATAL:
        // const char *format3 = "[%s] [%s] [PID:%4d] [TID:%4d] [%-s] [%s:%4d]"; // [日志级别] [时间] [进程id] [线程id] [文件名] [函数名:行号]
        sprintf(buffer, "[%s] [%s] [PID:%4d] [TID:%4d] [%-s] [%s:%4d]", log_level_str, log_event.time.c_str(), log_event.process_id,
                log_event.thread_id, log_event.file_name.c_str(), log_event.function_name.c_str(),
                log_event.line_num);
    default:
        break;
    }
    log_str = buffer;
}

void Logger::write_log(logEvent &log_event, LOG_LEVEL log_level, std::string log_path)
{
    str message;
    format_log(log_event, log_level, message);
    message += log_event.log_meaasge;
    if(pLog_appander_List.find(log_path)==pLog_appander_List.end())
    {
        std::cout<<"log file error,error here: "<<__func__<<__LINE__<<std::endl;
        return ;
    }
    
    pLog_appander_List[log_path]->write(message);
}


//暴露出的写日志接口,日志类改为单例模式后更改后基本没用了
void write_log(Logger &log_obj, str message, LOG_LEVEL log_level, std::string log_path)
{
    switch (log_level)
    {
    case INFO:
    case DEBUG:
    {
        logEvent event(message, WARNING);
        return log_obj.write_log(event, INFO, log_path);
    }
    case WARNING:
    {
        logEvent event(message, WARNING, getpid(), syscall(SYS_gettid));
        return log_obj.write_log(event, WARNING, log_path);
    }
    case ERROR:
    case FATAL:
        // const char *format3 = "[%s] [%s] [PID:%4d] [TID:%4d] [%-s] [%s:%4d]"; // [日志级别] [时间] [进程id] [线程id] [文件名] [函数名:行号]
        {
            logEvent event(message, ERROR, getpid(), syscall(SYS_gettid), __FILE__, __func__, __LINE__);
            return log_obj.write_log(event, ERROR, log_path);
        }
    default:
        break;
    }
}
//---------------------------------------------------->
