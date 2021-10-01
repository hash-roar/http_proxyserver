#include "tool.h"

std::vector<std::string> split_string(const std::string &str, const char pattern)
{
    std::vector<std::string> res;
    std::stringstream input(str);   //读取str到字符串流中
    std::string temp;
    //使用getline函数从字符串流中读取,遇到分隔符时停止,和从cin中读取类似
    //注意,getline默认是可以读取空格的
    while(std::getline(input, temp, pattern))
    {
        res.push_back(temp);
    }
    return res;
}