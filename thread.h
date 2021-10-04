#ifndef THREAD_H
#define THREAD_H
#include "tool.h"
#include "log.h"
#include "http_conn.h"
#include "config.h"
#include "server.h"

#include <thread>

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
    test_thread(bool is_deta = false){

    };
    ~test_thread(){

    };
};

class server_thread : public thread_base
{
private:
    void Main() override;
    server_config config;
    Logger *plogger;

public:
    server_thread(const server_config &config_obj, bool is_deta = false) : config(config_obj), thread_base(is_deta) { server_init(); }
    void server_init();
    ~server_thread();
};

#endif