#include "config.h"

//配置文件解析类实现
http_config::http_config(/* args */)
{
}

http_config::~http_config()
{
}


server_config::~server_config()
{
}

void server_config::add_http_config(std::string url, const http_config &ht_conf)
{
    httpconfig_lsit.insert(std::make_pair(url, ht_conf));
}

bool server_config::url_route(std::string url, http_config &htt_conf)
{
    for (auto item : httpconfig_lsit)
    {
        if (url.find(item.first) != std::string::npos)
        {
            htt_conf = item.second;
            return true;
        }
    }
    return false;
}



Config::Config(std::string conf_path)
{
    parse_config(conf_path);
}

Config::~Config()
{
}
void Config::parse_config(std::string conf_path)
{
    YAML::Node config = YAML::LoadFile(conf_path);
    YAML::Node http = config["http"];
    for (auto item1 : http)
    {
        auto item = item1["server"];
        std::string listen = std::to_string(item["listen"].as<int>());
        std::string server_name = item["server_name"].as<std::string>();
        server_config server_config_obj(listen, server_name);
        if (item["error_log"])
        {
            std::string error_log = item["error_log"].as<std::string>();
            server_config_obj.set_error_log(error_log);
        }
        if (item["access_log"])
        {
            std::string access_log = item["access_log"].as<std::string>();
            server_config_obj.set_access_log(access_log);
        }
        YAML::Node locations = item["locations"];
        for (auto node1 : locations)
        {
            auto node = node1["location"];
            http_config http_config_obj;
            if (node["root"])
            {
                http_config_obj.set_root(node["root"].as<std::string>());
            }
            if (node["index"])
            {
                http_config_obj.set_index(node["index"].as<std::string>());
            }
            if (node["proxy_pass"])
            {
                http_config_obj.set_proxy_pass(node["proxy_pass"].as<std::string>());
            }
            if (node["proxy_set_header"])
            {
                http_config_obj.set_proxy_set_header(node["proxy_set_header"].as<std::string>());
            }
            // for(auto key :node)
            // {

            // }
            server_config_obj.add_http_config(node["url"].as<std::string>(), http_config_obj);
        }
        server_list.insert(std::make_pair(server_name, server_config_obj));
    }
    parsing_succ_ = true;
}

bool Config::is_success()
{
    return parsing_succ_;
}
