
#include "./tools/tool.h"
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
// class log_
// {
// private:
// ;
// public:
//     static const char *log_list[];

// public:
//     log_(const char *logfile_name = "log/log_file");
//     bool log_write(const char *message, int level);
//     ~log_();
// };
//---------------------------------------------------->
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

private:
    std::map<std::string, std::string> header_info;
    int cgi_excu = 0;
    const int buffer_size = 10240;
    static const std::map<std::string, std::string> mime_types;

public:
    http_conn();
    ~http_conn();

public:
    void handle_request();
    void bad_request();
    void error_print(const char *mess);
    void exc_cgi();
    void send_header(std::map<std::string,std::string>& header,std::string &first_line);
    void fourOfour_error();
    void send_server_file();
    void parse_header(const std::string &s);
    std::string  parse_mime_type(const std::string &file_path);
    void parse_mime_type(const std::string &url, std::string &mime_type);
    int get_one_line(char *buffer, int size);
    void not_implemented();
    void resolve_get();
    void send_file(std::string file_path);
    void excute_cgi_error();
};
