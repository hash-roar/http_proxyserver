#include "./http_conn.h"

std::string http_conn::wwwroot_path = "/home/zlf/c++/newserver/wwwroot";
const std::map<std::string, std::string> http_conn::mime_types = {{"html", "text/html"}, {"txt", "text/plain"}, {"img", "image/jpeg"}, {"jpg", "image/jpeg"}, {"png", "image/jpeg"}};

http_conn::http_conn()
{
}

http_conn::~http_conn()
{
    close(fd);
}

bool http_conn::handle_request()
{
    char buffer[4096];
    bzero(buffer, sizeof(buffer));
    int head_length = recv(fd, buffer, sizeof(buffer), 0);
    if (head_length == 0)
    {
        std::cout << "读缓冲区错误" << std::endl;
        return false;
    }

    parse_header(buffer);
    if (http_info_obj.mehtod != "GET")
    {
        not_implemented();
        return false;
    }
    return true;
    //路由模块
}
void http_conn::handle_route()
{
    if (self_config.get_proxy_pass() == "")
    {
        resolve_get();
    }
    else
    {
        deb1("正在处理代理转发信息")
            handle_proxy();
    }
}

void http_conn::handle_proxy()
{
    std::string proxy_pass = self_config.get_proxy_pass();
    std::string domain;
    std::string port;
    std::string proxy_pass_url = http_info_obj.uri;
    //解析域名及端口
    auto pos = proxy_pass.find_first_of("//");
    proxy_pass = proxy_pass.substr(pos + 2);
    if (proxy_pass.find("/") != std::string::npos)
    {
        auto pos = proxy_pass.find_first_of("/");
        proxy_pass_url = proxy_pass.substr(pos);
        proxy_pass = proxy_pass.substr(0, pos);
    }
    if (proxy_pass.find_last_of(":") == std::string::npos)
    {
        domain = proxy_pass;
        port = "80";
    }
    else
    {
        auto pos = proxy_pass.find_last_of(":");
        domain = proxy_pass.substr(0, pos);
        port = proxy_pass.substr(pos + 1);
    }
    //开启与后端连接
    deb1("正在建立到后端服务器连接")
        client_conn client_obj(domain, std::stoi(port));
    forward_all_data(client_obj, proxy_pass_url);
}

void http_conn::forward_all_data(client_conn &client_obj, std::string &proxy_pass_url)
{
    //发送请求头
    char buffer[4096];
    sprintf(buffer, "%s\r\n", (http_info_obj.mehtod + " " + proxy_pass_url + " " + http_info_obj.http_version).c_str());
    send(client_obj.get_fd(), buffer, strlen(buffer), 0);
    std::string key;
    std::string value;
    if (self_config.get_proxy_set_header() != "")
    {
        std::string set_header = self_config.get_proxy_set_header();
        key = split_string(set_header, ' ')[0];
        value = split_string(set_header, ' ')[1];
    }
    for (auto item : header_info)
    {
        if (item.first == key)
        {
            sprintf(buffer, "%s: %s\r\n", item.first.c_str(), value.c_str());
            send(client_obj.get_fd(), buffer, strlen(buffer), 0);
        }
        else
        {
            sprintf(buffer, "%s: %s\r\n", item.first.c_str(), item.second.c_str());
            send(client_obj.get_fd(), buffer, strlen(buffer), 0);
        }
    }
    sprintf(buffer, "\r\n");
    send(client_obj.get_fd(), buffer, strlen(buffer), 0);
    //接受后端响应头
    client_obj.get_header();
    //发送响应头到前端
    sprintf(buffer, "%s\r\n", client_obj.head_line.c_str());
    send(fd, buffer, strlen(buffer), 0);

    for (auto item : client_obj.head_info)
    {
        sprintf(buffer, "%s: %s\r\n", item.first.c_str(), item.second.c_str());
        send(fd, buffer, strlen(buffer), 0);
    }
    sprintf(buffer, "\r\n");
    send(fd, buffer, strlen(buffer), 0);
    //接下来转发两个套接字所有数据
    char buf[4096];
    while (true)
    {
        int length = recv(client_obj.get_fd(), buffer, sizeof(buffer), 0);
        if (length == 0)
        {
            deb1("后端服务器断开连接") break;
        }
        send(fd, buffer, length, 0);
    }
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
    auto &url = head_first_line_infos[1];
    if (url.find("?") != std::string::npos)
    {
        auto pos = url.find("?");
        http_info_obj.uri = url.substr(0, pos);
        query_string = url.substr(pos + 1);
    }
    http_info_obj.uri = url;
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
    std::string file_path = self_config.get_root() + http_info_obj.uri;
    char path_end = *(file_path.rbegin());
    std::string mime_type;
    if (path_end == '/')
    {
        file_path += self_config.get_index();
    }
    struct stat st;
    if (stat(file_path.c_str(), &st) == -1)
    {
        fourOfour_error();
    }
    else if (st.st_mode & S_IFDIR) //请求文件为目录
    {
        file_path = file_path + "/" + self_config.get_index();
    }

    else
    {
        mime_type = parse_mime_type(file_path);
    }
    mapType header{{"Server", "bingyan_server/0.1.0"}, {"Content-Type", mime_type.c_str()}};
    std::string first_line = "HTTP/1.0 200 OK";
    send_header(header, first_line);
    deb1(file_path);
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
    sprintf(buf, "<body><h1>你要的,它的归属不在这里.</h1><h1> 世界终将404</h1>\r\n");
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
    auto pos = file_path.find_last_of(".");
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
std::string http_conn::get_http_info(const std::string &key)
{
    std::string value = "";
    if (header_info.find(key) != header_info.end())
    {
        value = header_info[key];
    }
    return value;
}
//----------------------------------------------------------->

client_conn::client_conn(std::string host1, int pt)
{
    host_str = host1;
    port = pt;
    if (init_connect() == -1)
    {
        deb1("与后端服务器连接失败")
    }
    else
    {
        deb1("与后端服务器连接成功")
    }
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
    inet_pton(AF_INET, host_str.c_str(), &saddr.sin_addr.s_addr);
    int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret == -1)
    {
        perror("connet");
        return -1;
    }
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
    return header;
}
void client_conn::parse_header(const std::string &head_str)
{
    // std::cout << head_str << std::endl;
    std::istringstream head_steam(head_str.c_str());
    std::string head_first_line;
    std::getline(head_steam, head_first_line);
    head_line = head_first_line;
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
            std::cout << "the server close the connection" << std::endl;
        }
        send(to_front->get_fd(), buffer, length, 0);
    }
}

proxy::~proxy()
{
}
