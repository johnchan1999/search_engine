#include <nlohmann/json.hpp>
#include "../Include/DealJson.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using std::cin;
using std::cout;
using std::endl;
using std::isdigit;
using std::stoi;
using std::string;
using std::strncpy;

#define SERV_IP "127.0.0.1"
#define SERV_PORT 8899

int main()
{
    int cfd;
    struct sockaddr_in serv_addr;
    char buf[65563];

    int nByte;
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == cfd)
    {
        perror("socket error");
        exit(-1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /* inet_pton(cfd, SERV_IP, &serv_addr.sin_addr.s_addr); */
    connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    DealJson transJson;       // 声明一个json类对象
    map<int, string> content; // 一个map容器用于接收返回内容

    while (1)
    {
        int operation;
        std::cout << "请选择操作: (1) 关键词推荐 (2) 搜索功能" << std::endl;
        std::cin >> operation;

        string queryWord;
        cin >> queryWord; // 保存用户在开始输入的查询词

        content.clear();
        content = transJson.initJson(1, queryWord); // 获取打包后的json串
        content[1].append({'\n'});
        strncpy(buf, content[1].c_str(), sizeof(buf) - 1);
        write(cfd, buf, strlen(buf)); // 查询json串传给服务器

        std::cout << ">> 发送关键词完成：" << buf << std::endl;

        nByte = read(cfd, buf, sizeof(buf)); // 获取服务器传回的关键字集合json串
        std::cout << ">> 接收查询结果：" << buf << std::endl;
        string ktmp(buf); // buf转为string
        content.clear();
        content = transJson.initJson(100, ktmp); // 获取解包后关键字集合map

        switch (operation)
        {
        case 1:
            std::cout << ">> 关键词推荐: ";
            // 输出关键字集合
            for (auto &elem : content)
            {
                std::cout << elem.first << ":" << elem.second << "  ";
            }
            std::cout << std::endl;
            break;

        case 2:
            // 在此执行搜索功能代码
            std::cout << ">> 搜索结果: ";
            for (auto &elem : content)
            { // 输出网页集合
                std::cout << elem.second << std::endl;
            }
            break;

        default:
            std::cout << "输入无效，请输入1或2." << std::endl;
            continue;
        }
        bzero(buf, sizeof(buf));
    }
    close(cfd);
    return 0;
}