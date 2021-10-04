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

std::vector<std::string> split_string(const std::string &str, const char pattern);
int get_one_line(int fd,char *buffer, int size);
std::string get_local_time();
#endif