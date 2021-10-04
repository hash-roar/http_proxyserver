#include "thread.h"
Logger *Logger::log_obj_ = nullptr;


void server_thread::Main()
{
    server server_obj(std::stoi(config.get_listen_port()));
    while (!is_exit())
    {
        http_conn http_obj;
        http_obj.fd = accept(server_obj.get_fd(), (struct sockaddr *)&http_obj.saddr, &http_obj.addrlen);
        if (http_obj.fd == -1)
        {
            perror("accept");
            // log_obj.log_write("accept error",ERROR);
            exit(-1);
        }
        if (!http_obj.handle_request()) //当接收到的请求不是get请求时,写入日志.由not impleted方法处理
        {
            logEvent log_event("非GET方法访问", WARNING, 0, getpid());
            plogger->write_log(log_event, WARNING, config.get_error_log());
            continue;
        }
        //进行路由
        http_config self_config;
        // 写入日志
        logEvent log_event(http_obj.head_line,INFO);
        plogger->write_log(log_event,INFO,config.get_access_log());
        if (config.url_route(http_obj.http_info_obj.uri, self_config)) //对路由进行匹配
        {
            http_obj.set_self_config(self_config);
        }
        else
        {
            http_obj.resolve_get();
        }
    }
}
void server_thread::server_init()
{
    plogger = Logger::get_log_obj();
    plogger->add_logappender(config.get_access_log());
    plogger->add_logappender(config.get_error_log());
}
server_thread::~server_thread()
{
}
