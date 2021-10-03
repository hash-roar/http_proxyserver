#include "./http_conn.h"
// server ------------------------------------------->

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

//----------------------------------------------------------->

std::string http_conn::wwwroot_path = "/home/zlf/c++/proxy_server/wwwroot";
const std::map<std::string, std::string> http_conn::mime_types = {{"html", "text/html"}, {"txt", "text/plain"}, {"img", "image/jpeg"}, {"jpg", "image/jpeg"}, {"png", "image/jpeg"}};

http_conn::http_conn()
{
}

http_conn::~http_conn()
{
    close(fd);
}

void http_conn::handle_request()
{
    char buffer[4096];
    bzero(buffer, sizeof(buffer));
    int head_length = recv(fd, buffer, sizeof(buffer), 0);
    parse_header(buffer);
    if (http_info_obj.mehtod != "GET")
    {
        not_implemented();
        return;
    }
    //路由模块
    resolve_get();
}

void http_conn::parse_header(const std::string &head_str)
{
    // std::cout << head_str << std::endl;
    std::istringstream head_steam(head_str.c_str());
    std::string head_first_line;
    std::getline(head_steam, head_first_line);
    head_line = head_first_line;
    std::vector<std::string> head_first_line_infos;
    //解析第一行
    head_first_line_infos = split_string(head_first_line, ' ');
    http_info_obj.mehtod = head_first_line_infos[0];
    http_info_obj.uri = head_first_line_infos[1];
    http_info_obj.http_version = head_first_line_infos[2];
    //解析剩下所有,放入map中
    std::string head;
    while (std::getline(head_steam, head) && head != "\r")
    {
        int index = head.find_first_of(':');
        if (index != std::string::npos)
        {
            header_info.insert(std::make_pair(head.substr(0, index), head.substr(index + 2)));
        }
    }
    // for (auto &kv : header_info)
    // {
    //     std::cout<<"fisr:"<<kv.first<<"---last:"<<kv.second<<std::endl;
    // }
}

void http_conn::not_implemented()
{
    char buf[1024];
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "Server: http_server/0.1.0\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "Content-Type: text/html\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "</TITLE></HEAD>\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "</BODY></HTML>\r\n");
    send(fd, buf, strlen(buf), 0);
}

void http_conn::resolve_get()
{
    std::string file_path = wwwroot_path + http_info_obj.uri;
    char path_end = *(file_path.rbegin());
    std::string mime_type;
    if (path_end == '/')
    {
        file_path += "index.html";
    }
    struct stat st;
    if (stat(file_path.c_str(), &st) == -1)
    {
        fourOfour_error();
    }
    else
    {
        mime_type = parse_mime_type(file_path);
    }
    mapType header{{"Server", "bingyan_server/0.1.0"}, {"Content-Type", mime_type.c_str()}};
    std::string first_line = "HTTP/1.0 200 OK";
    send_header(header, first_line);
    send_file(file_path);
}

void http_conn::send_file(std::string file_path)
{
    struct stat st;
    char buf[1024] = {0};
    if (stat(file_path.c_str(), &st) == -1)
    {
        server_error();
        return;
    }
    else
    {
        int content_length = st.st_size;
        sprintf(buf, "content-length: %d\r\n", content_length);
        send(fd, buf, strlen(buf), 0);
        sprintf(buf, "\r\n");
        send(fd, buf, strlen(buf), 0);
        int file_fd = open(file_path.c_str(), O_RDONLY);
        if (file_fd < 0)
        {
            server_error();
            return;
        }
        sendfile(fd, file_fd, NULL, content_length);
        close(file_fd);
        return;
    }
}

void http_conn::server_error()
{
    char buf[1024] = {0};
    sprintf(buf, "\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "<html><title>error</title><meta http-equiv='Content-Type' content='text/html'; charset='UTF-8'>\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "<body><h1>服务端发生错误!</h1>\r\n");
    send(fd, buf, strlen(buf), 0);
    sprintf(buf, "</body></html>\r\n");
    send(fd, buf, strlen(buf), 0);
}
void http_conn::fourOfour_error()
{
    char buf[1024];
    std::string head_first_line = "HTTP/1.0 404 NOT FOUND";
    mapType header{{"Server", "bingyan server/0.1.0"}, {"Content-Type", "text/html"}};
    send_header(header, head_first_line);
    send_file(wwwroot_path + "/404.html");
}

std::string http_conn::parse_mime_type(const std::string &file_path)

{
    auto pos = file_path.find('.');
    std::string suffix = file_path.substr(pos + 1, (file_path.size() - pos - 1));
    auto mime_type_ptr = http_conn::mime_types.find(suffix);
    std::string mime_type;
    if (mime_type_ptr == http_conn::mime_types.end())
    {
        mime_type = "text/plain";
    }
    else
    {
        mime_type = mime_type_ptr->second;
    }
    return mime_type;
}

void http_conn::send_header(std::map<std::string, std::string> &header, std::string &first_line)
{
    char buffer[1024] = {0};
    sprintf(buffer, "%s\r\n", first_line.c_str());
    send(fd, buffer, strlen(buffer), 0);
    for (auto &item : header)
    {
        sprintf(buffer, "%s: %s\r\n", item.first.c_str(), item.second.c_str());
        send(fd, buffer, strlen(buffer), 0);
    }
}
//----------------------------------------------------------->

client_conn::client_conn(std::string host1, int port1) : host(host1), port(port1)
{
}

client_conn::~client_conn()
{
    close(fd);
}
int client_conn::init_connect()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        return -1;
    }
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &saddr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connet");
        return -1;
    }
    std::cout << "与server连接成功" << std::endl;
    return 1;
}

// void client_conn::send_request(char* buffer,int length)
// {

// }
std::string client_conn::get_header()
{
    char buffer[1024] = {0};
    std::string temp;
    std::string header;
    do
    {
        get_one_line(fd, buffer, sizeof(buffer));
        header += buffer;
        temp = buffer;
    } while (temp != "\n");
    parse_header(header);
}
void client_conn::parse_header(const std::string &head_str)
{
    // std::cout << head_str << std::endl;
    std::istringstream head_steam(head_str.c_str());
    std::string head_first_line;
    std::getline(head_steam, head_first_line);
    std::vector<std::string> head_first_line_infos;
    //解析第一行
    head_first_line_infos = split_string(head_first_line, ' ');
    response_info_obj.http_version = head_first_line_infos[0];
    size_t *pp = (size_t *)malloc(sizeof(size_t));
    response_info_obj.status = std::stoi(head_first_line_infos[1], pp, 10);
    response_info_obj.status_text = head_first_line_infos[2];
    //解析剩下所有,放入map中
    std::string head;
    while (std::getline(head_steam, head) && head != "\r")
    {
        int index = head.find_first_of(':');
        if (index != std::string::npos)
        {
            head_info.insert(std::make_pair(head.substr(0, index), head.substr(index + 2)));
        }
    }
    // for (auto &kv : header_info)
    // {
    //     std::cout<<"fisr:"<<kv.first<<"---last:"<<kv.second<<std::endl;
    // }
}
//---------------------------------------------------------->
//代理服务器维护一个与客户端的socket连接,维护一个与后台服务器的连接并将所有后台数据转发给前台
proxy::proxy(http_conn *to_front, client_conn *to_back)
{
    this->to_front = to_front;
    this->to_back = to_back;
}

void proxy::response_header()
{
    std::string first_line = to_back->response_info_obj.http_version + std::to_string(to_back->response_info_obj.status) + to_back->response_info_obj.status_text;
    to_back->head_info["server"] = "bingyan/0.1.0";
    send_header(to_back->head_info, first_line);
}
void proxy::send_header(std::map<std::string, std::string> &header, std::string &first_line)
{
    char buffer[1024] = {0};
    sprintf(buffer, "%s\r\n", first_line.c_str());
    send(to_front->get_fd(), buffer, strlen(buffer), 0);
    for (auto &item : header)
    {
        sprintf(buffer, "%s: %s\r\n", item.first.c_str(), item.second.c_str());
        send(to_front->get_fd(), buffer, strlen(buffer), 0);
    }
}
void proxy::forward_data()
{
    char buffer[4096];
    while (true)
    {
        int length = recv(to_back->get_fd(), buffer, sizeof(buffer), 0);
        if (length == 0)
        {
            out("服务端断开连接") break;
        }
        send(to_front->get_fd(), buffer, length, 0);
    }
}

proxy::~proxy()
{
}

//---------------------------------------------------------------------------->
//配置文件解析类实现

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
    for (auto item : http)
    {
        std::string listen = item["listen"].as<std::string>();
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
        for (auto node : locations)
        {
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

http_config::http_config(/* args */)
{
}

http_config::~http_config()
{
}
