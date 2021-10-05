#ifndef TOOL_H
#define TOOL_H
#include <vector>
#include <string>
#include <string.h>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <map>
#include <stdio.h>
#include <arpa/inet.h>
#include <netdb.h>

#define deb(x)  print("%s\n",x);
#define deb1(x) std::cout<<x<<std::endl;


std::vector<std::string> split_string(const std::string &str, const char pattern);
int get_one_line(int fd,char *buffer, int size);
std::string get_local_time();
std::string get_ip_by_domain(std::string);
#endif