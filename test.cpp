#include "tool.h"
// #include "http_conn.h"
#include <regex>

using namespace std;
using str = std::string;
int main1(int arg, char **args)
{

    // cout << args[1] << endl;
    // string domain = args[1];
    // cout<<get_ip_by_domain(domain);
    // std::string domain;
    // std::string port;
    // client_conn client_obj("127.0.0.1",8999);
    // std::string fnames[] = {"foo.txt", "bar.txt", "test", "a0.txt", "AAA.txt"};
    // 在 C++ 中 \ 会被作为字符串内的转义符，为使 \. 作为正则表达式传递进去生效，需要对 \ 进行二次转义，从而有 \\.
    // std::regex txt_regex("[a-z]+\\.txt");
    // for (const auto &fname : fnames)
    //     std::cout << fname << ": " << std::regex_match(fname, txt_regex) << std::endl;
    // string domain = "/api";
    // regex regex_text("^/api.*");
    // cout<<regex_match(domain,regex_text)<<endl;
    // string url = "= /api/";
    // auto &&str_vc = move(split_string(url, ' '));

    // // string uri = split_string(url, ' ')[1];
    // cout << str_vc.size() << endl;
    str url = "/index?name=zlf";
    if (url.find("?") != std::string::npos)
    {
        auto pos = url.find("?");
        deb1(url.substr(0, pos))
            deb1(url.substr(pos + 1))
    }
    return 0;
}