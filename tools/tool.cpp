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

int get_one_line(int fd,char *buffer, int size)
{
    int i = 0;
    char c = '\0';
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        //recv()包含于<sys/socket.h>,参读《TLPI》P1259,
        //读一个字节的数据存放在 c 中
        n = recv(fd, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')
            {
                //
                n = recv(fd, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(fd, &c, 1, 0);
                else
                    c = '\n';
            }
            buffer[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buffer[i] = '\0';

    return (i);
}

std::string get_local_time()
{
    time_t t = time(0);
    char temp[32]={NULL};
    strftime(temp, sizeof(temp), "%Y-%m-%d %H:%M:%S",localtime(&t)); 
    return temp;
}