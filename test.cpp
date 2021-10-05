#include "tool.h"
#include "http_conn.h"

using namespace std;
int main1(int arg, char **args)
{

    // cout << args[1] << endl;
    // string domain = args[1];
    // cout<<get_ip_by_domain(domain);
    std::string domain;
    std::string port;
    client_conn client_obj("127.0.0.1",8999);
    
   

    cout << domain << "        " << port << endl;
    return 0;
}