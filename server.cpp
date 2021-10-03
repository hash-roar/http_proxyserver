#include "./http_conn.h"
#include "./log.h"

Logger log_obj("./logs/log.txt");

int main()
{
    //1
    //main函数开始,加载配置文件,
    Config main_config("config.yaml");

    //2
    //开启控制进程和工作线程(屏蔽sigpipe信号,初始化服务器套接字)
    test_thread thread1;
    thread1.start();
    thread1.wait();
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