#ifndef SERVER_H
#define SERVER_H
#include "tool.h"
#include <arpa/inet.h>
#include <unistd.h>

class server
{
private:
    int fd;
    int port;
    struct sockaddr_in saddr;

public:
    server(int port = 8999, const char *ip = INADDR_ANY);
    ~server();
    void server_sock_init(int port, const char *ip);
    int get_fd();
};


#endif