#include "./http_conn.h"


int main()
{
    server server_obj; //初始化服务器,开启套接字并监听端口
    http_conn http_obj;
    http_obj.fd = accept(server_obj.get_fd(),(struct sockaddr *)&http_obj.saddr, &http_obj.addrlen);
    if (http_obj.fd == -1)
        {
            perror("accept");
            // log_obj.log_write("accept error",ERROR);
            exit(-1);
        }
    std::cout<<"http_connect init successfully"<<std::endl;
    http_obj.handle_request();

}