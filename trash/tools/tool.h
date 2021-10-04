#ifndef TOOL_H
#define TOOL_H
#include "./head_files.h"
std::vector<std::string> split_string(const std::string &str, const char pattern);
int get_one_line(int fd,char *buffer, int size);
std::string get_local_time();
#endif