#include "thread.h"
#include "config.h"
using namespace std;

int main(int arg, char **args)
{
    string config_file_path = arg == 1 ? "config.yaml" : args[1]; //读取配置文件path
    //初始化配置
    Config main_cofig(config_file_path);
    if (!main_cofig.is_success()) //若加载配置不成功
    {
        cout << "config load error!" << endl;
        return 0;
    }
    cout << "config parsing successfully" << endl;
    // 初始化日志类实例(单例模式)
    Logger * plog_obj = Logger::get_log_obj();

    //初始化服务器,开启多线程
    int server_num = main_cofig.get_server_list().size();
    server_thread * pserver_thread[server_num] = {nullptr};
    int i=0;
    for (auto item : main_cofig.get_server_list())
    {
        pserver_thread[i]= new server_thread(item.second);
        if (pserver_thread[i]==nullptr) //服务器初始化错误写入主错误日志
        {
            logEvent log_event("server "+item.second.get_server_name()+" init failed",ERROR,
            0,getpid(),__FILE__,__func__,__LINE__);
            plog_obj->write_log(log_event,ERROR,"logs/main_log.txt");
        }
        //开启服务器
        pserver_thread[i]->start();
        cout<<"server "+item.second.get_server_name()+" start"<<endl;
        i++;
    }
    for (int j=0;i<=i;j++)
    {
        pserver_thread[j]->wait(); //主线程等待永远不会结束的子线程
    }

    return 0;
}
