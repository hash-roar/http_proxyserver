
#include "./tools/tool.h"
using mapType = std::map<std::string, std::string>;
#define out(x) std::cout << x << std::endl;

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
    std::string head_line;

private:
    std::map<std::string, std::string> header_info;
    int cgi_excu = 0;
    const int buffer_size = 10240;
    static const std::map<std::string, std::string> mime_types;

public:
    http_conn();
    ~http_conn();
    int get_fd()
    {
        return fd;
    }

public:
    void handle_request();
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
class thread_base
{
private:
    virtual void Main() = 0;
    std::thread thread_;
    bool is_exit_;
    bool is_detach_;

public:
    thread_base(bool is_detach = false) : is_detach_(is_detach)
    {
    }
    ~thread_base(){};

public:
    virtual void start()
    {
        is_exit_ = false;
        thread_ = std::thread(&thread_base::Main, this);
    }
    virtual void stop()
    {
        is_exit_ = true;
        wait();
    }
    virtual void wait()
    {
        if (thread_.joinable())
            thread_.join();
    }
    bool is_exit() { return is_exit_; }
};

class test_thread : public thread_base
{
private:
    void Main() override
    {
        while (!is_exit())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "this test" << std::endl;
        }
    }

public:
    test_thread(bool is_deta){

    };
    ~test_thread(){

    };
};

class server_thread : public thread_base
{
private:
    void Main() override;
    server_config config;

public:
    server_thread(const server_config &config_obj) : config(config_obj){};
    ~server_thread();
};

//---------------------------------------------------------------->
//加载配置及解析配置
class http_config
{
private:
    /* data */
    std::string root = "./wwwroot";
    std::string index = "index.html";
    std::string proxy_pass = "";
    std::string proxy_set_header = "";
    std::map<std::string, std::string> config_map;

public:
    http_config(/* args */);
    void set_root(std::string rt) { root = rt; }
    void set_index(std::string inde) { index = inde; }
    void set_proxy_pass(std::string pro_pa) { proxy_pass = pro_pa; }
    void set_proxy_set_header(std::string pro_set_he) { proxy_set_header = pro_set_he; }
    ~http_config();
};

class server_config
{
private:
    std::string listen;
    std::string server_name;
    std::string access_log = "./logs/access_logs.txt";
    std::string error_log = "./logs/error_logs.txt";
    std::map<std::string, http_config> httpconfig_lsit;

public:
    // server_config(std::string ls, std::string se_nam, std::string a_lg = "/home/zlf/c++/logs/acees_logs.txt", std::string e_lg = "/home/zlf/c++/logs/error_logs.txt")
    //     : listen(ls), server_name(se_nam), acceess_log(a_lg), error_log(e_lg)
    // {
    // }
    server_config(std::string ls, std::string se_nam) : listen(ls), server_name(se_nam)
    {
    }
    void set_access_log(std::string acc_log) { access_log = acc_log; }
    void set_error_log(std::string err_log) { err_log = err_log; }
    void add_http_config(std::string url, const http_config &ht_conf);
    bool url_route(std::string url, http_config &htt_conf);
    ~server_config();
};

class Config
{
private:
    std::map<std::string, server_config> server_list;

public:
    Config(std::string conf_path);
    void parse_config(std::string conf_path);
    const std::map<std::string, server_config> &get_server_list() { return server_list; };
    ~Config();
};
