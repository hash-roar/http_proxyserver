#ifndef CONFIG_H
#define CONFIG_H
#include "./tool.h"
#include <yaml-cpp/yaml.h>

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
    std::string get_server_name() { return server_name; }
    std::string get_listen_port() { return listen; }
    std::string get_access_log() { return access_log; }
    std::string get_error_log() { return error_log; }
    ~server_config();
};

class Config
{
private:
    std::map<std::string, server_config> server_list;
    bool parsing_succ_ =false;

public:
    Config(std::string conf_path);
    void parse_config(std::string conf_path);
    const std::map<std::string, server_config> &get_server_list() { return server_list; };
    bool is_success();
    ~Config();
};


#endif