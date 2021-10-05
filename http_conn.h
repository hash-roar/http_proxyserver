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
    //套接字数据
    int fd;
    struct sockaddr_in saddr;
    //连接数据
    std::string host_str;
    int port;

public:
    //http头数据
    mapType head_info;
    response_info response_info_obj;
    std::string head_line;

public:
    client_conn(std::string host_str, int pt);
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
//------------------------------------------------------------------------>

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
    std::string query_string="";

private:
    std::map<std::string, std::string> header_info;
    static const std::map<std::string, std::string> mime_types;
    int cgi_excu = 0;
    const int buffer_size = 10240;
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
    //根据配置处理请求
    bool handle_request();
    void handle_route();
    void resolve_get();
    void handle_proxy();
    //为写日志创造的接口
    std::string get_http_info(const std::string &key);

private:
    //解析
    int get_one_line(char *buffer, int size);
    void parse_header(const std::string &s);
    std::string parse_mime_type(const std::string &file_path);
    void parse_mime_type(const std::string &url, std::string &mime_type);
    //响应错误
    void not_implemented();
    void fourOfour_error();
    void server_error();
    //发送响应
    void send_header(std::map<std::string, std::string> &header, std::string &first_line);
    void send_file(std::string file_path);
    //转发数据
    void forward_all_data(client_conn &cilent_obj,std::string &proxy_pass_url);
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