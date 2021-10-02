#include "./log.h"

//-------------------------------------------->

//---------------------------------------------->

void log::format_log(logEvent &log_event, LOG_LEVEL log_level, str &log_str)
{
    char buffer[512];
    const char *log_level_str = loglevel_map[log_level].c_str();
    switch (log_level)
    {
    case INFO:
        // char *format = "[%s] [%s]";
        // sprintf(buffer, format, log_level_str, log_event.time);
        // break;
    case DEBUG:
        char *format = "[%s] [%s]";
        sprintf(buffer, format, log_level_str, log_event.time);
        break;
    case WARNING:
        char *format = "[%s] [%s] [PID:%4d] [TID:%4d]";
        sprintf(buffer, format, log_level_str, log_event.time, std::to_string(log_event.process_id).c_str(), std::to_string(log_event.thread_id).c_str());
        break;
    case ERROR:
        //  char *format = "[%s] [%s] [PID:%4d] [TID:%4d] [%-s] [%s:%4d]";         // [日志级别] [时间] [进程id] [线程id] [文件名] [函数名:行号]
    case FATAL:
        char *format = "[%s] [%s] [PID:%4d] [TID:%4d] [%-s] [%s:%4d]"; // [日志级别] [时间] [进程id] [线程id] [文件名] [函数名:行号]
        sprintf(buffer, format, log_level_str, log_event.time.c_str(), std::to_string(log_event.process_id).c_str(), 
        std::to_string(log_event.thread_id).c_str(),log_event.file_name.c_str(),log_event.function_name.c_str(),
        std::to_string(log_event.line_num).c_str());
    default:
        break;
    }
    log_str = buffer;
}



//---------------------------------------------------->

void logAppender::write(str & meaasge)
{
    pthread_mutex_lock(&file_mutex);
    file_stream.write(meaasge.c_str(),meaasge.size());
    file_stream<<"\n";
    pthread_mutex_unlock(&file_mutex);
}