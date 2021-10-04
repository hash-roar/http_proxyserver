#ifndef HTTP_CONN_H
#define HTTP_CONN_H
#include "./tool.h"
#include "config.h"
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
using mapType = std::map<std::string, std::string>;
typedef struct http_info
{
    std::string mehtod;
    std::string uri;
    std::string http_version;
} http_info;

class http_conn
{
public:
    int fd;
    struct sockaddr_in saddr;
    socklen_t addrlen = sizeof(struct sockaddr);
    static std::string wwwroot_path;
    http_info http_info_obj;
    std::string head_line;

private:
    std::map<std::string, std::string> header_info;
    int cgi_excu = 0;
    const int buffer_size = 10240;
    static const std::map<std::string, std::string> mime_types;
    http_config self_config;

public:
    http_conn();
    ~http_conn();
    int get_fd()
    {
        return fd;
    }

public:
    void set_self_config(http_config &config) { self_config = config; };
    bool handle_request();
    void bad_request();
    void error_print(const char *mess);
    void exc_cgi();
    void send_header(std::map<std::string, std::string> &header, std::string &first_line);
    void fourOfour_error();
    void send_server_file();
    void parse_header(const std::string &s);
    std::string parse_mime_type(const std::string &file_path);
    void parse_mime_type(const std::string &url, std::string &mime_type);
    int get_one_line(char *buffer, int size);
    void not_implemented();
    void resolve_get();
    void send_file(std::string file_path);
    void excute_cgi_error();
    void server_error();
    std::string get_host();
};

//-------------------------------------------------------------->
//作为客户端连接的类
typedef struct response_info
{
    std::string http_version;
    int status;
    std::string status_text;
} response_info;

class client_conn
{
private:
    int fd;
    std::string host;
    int port;
    struct sockaddr_in saddr;

public:
    mapType head_info;
    response_info response_info_obj;

public:
    client_conn(std::string host, int port);
    ~client_conn();
    int init_connect();
    int get_fd()
    {
        return fd;
    }
    // void send_request(char *buffer,int length);
    std::string get_header();
    void parse_header(const std::string &head_str);
};

//----------------------------------------------------------------------->
class proxy
{
private:
    std::string front_url;
    std::string back_url;
    http_conn *to_front;
    client_conn *to_back;

public:
    proxy(http_conn *to_front, client_conn *to_back);
    ~proxy();
    void response_header();
    void forward_data();
    void send_header(std::map<std::string, std::string> &header, std::string &first_line);
};

//----------------------------------------------------------->

#endif