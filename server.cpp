#include "server.h"

server::server(int port, const char *ip)
{
    this->server_sock_init(port, ip);
    this->port = port;
}
server::~server()
{
    close(fd);
}

void server::server_sock_init(int port, const char *ip)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
    {
        perror("socket");
        // echo_log("create socket error", ERROR);
        exit(-1);
    }

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("bind");
        // echo_log("bind socket error", ERROR);
        exit(-1);
    }
    ret = listen(fd, 100);
    if (ret == -1)
    {
        perror("listen");
        // echo_log("listen error", ERROR);
        exit(-1);
    }
    return;
}

int server::get_fd()
{
    return fd;
}
