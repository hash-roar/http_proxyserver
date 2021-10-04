#include "./log.h"
#include "./http_conn.h"
#include "unistd.h"
Logger *Logger::log_obj_ = nullptr;
class server_thread : public thread_base
{
private:
    void Main() override;
    server_config config;
    Logger *plogger;

public:
    server_thread(const server_config &config_obj, bool is_deta = false) : config(config_obj), thread_base(is_deta) { server_init(); }
    void server_init();
    ~server_thread();
};

void server_thread::Main()
{
    server server_obj(std::stoi(config.get_listen_port()));
    while (!is_exit())
    {
        http_conn http_obj;
        logEvent log_event("this is for test", INFO);
        plogger->write_log(log_event, INFO, config.get_access_log());
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
        http_config last_config;
        if (config.url_route(http_obj.http_info_obj.uri,last_config)) //对路由进行匹配
        {
            /* code */
        }
        else{
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

int main()
{
    //1
    //main函数开始,加载配置文件,
    Config main_config("config.yaml");

    //2
    //开启控制进程和工作线程(屏蔽sigpipe信号,初始化服务器套接字)
    for (auto item : main_config.get_server_list())
    {
        server_thread thread(item.second);
        thread.start();
        thread.wait();
    }
    std::this_thread::sleep_for(std::chrono::seconds(4));
    //3
    //进程进入主循环,开启等待连接

    //4
    //多线程处理连接请求,线程为每一个连接维护一个http_conn对象.开始定时器

    // server server_obj; //初始化服务器,开启套接字并监听端口
    // while (true)
    // {
    //     http_conn http_obj;
    //     http_obj.fd = accept(server_obj.get_fd(), (struct sockaddr *)&http_obj.saddr, &http_obj.addrlen);
    //     if (http_obj.fd == -1)
    //     {
    //         perror("accept");
    //         // log_obj.log_write("accept error",ERROR);
    //         exit(-1);
    //         write_log(log_obj, "socket accept error", ERROR);
    //     }
    //     std::cout << "http_connect init successfully" << std::endl;
    //     http_obj.handle_request();
    //     write_log(log_obj, http_obj.head_line, INFO);
    // }
}
