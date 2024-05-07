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
        string queryWord;
        string tmp; // 获取用户输入
        cin >> tmp;
        queryWord = tmp; // 保存用户在开始输入的查询词

        // 此处读入string后，可能存留换行符，考虑get清理

        content.clear();
        content = transJson.initJson(1, tmp); // 获取打包后的json串
        content[1].append({'\n'});
        strncpy(buf, content[1].c_str(), sizeof(buf) - 1);
        write(cfd, buf, strlen(buf)); // 查询json串传给服务器
        std::cout << ">> 发送关键词完成：" << buf << std::endl;

        nByte = read(cfd, buf, sizeof(buf)); // 获取服务器传回的关键字集合json串
        std::cout << ">> 接收查询结果：" << buf << std::endl;
        string ktmp(buf); // buf转为string
        content.clear();
        content = transJson.initJson(100, ktmp); // 获取解包后关键字集合map
        cout << ">> 请选择关键词: ";
        for (auto &elem : content)
        { // 输出关键字集合
            cout << elem.first << ":" << elem.second << "  ";
        }
        cout << endl;

        char ch;
        while (1)
        {
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 该行代码清空缓冲区内容直至遇见换行符（证明此前缓冲区里确实有\n）
            cin.get(ch);
            if (ch == '\n')
            {
                content.clear();
                content = transJson.initJson(2, queryWord);
                content[1].push_back('\n');
                strncpy(buf, content[1].c_str(), sizeof(buf) - 1);
                write(cfd, buf, strlen(buf)); // 关键json串传给服务器
                break;
            }
            else if (isdigit(ch))
            { // 此处有健壮性问题，若输入数字不在可选范围内如何
                int num;
                num = stoi(string(1, ch));
                if (num != 0 && content.count(num))
                { // 如果数字数字不为0，且在容器中
                    string selectWord = content[num];
                    content.clear();
                    content = transJson.initJson(2, selectWord); // 用户选定关键词 生成关键词json串
                    content[1].push_back('\n');
                    strncpy(buf, content[1].c_str(), sizeof(buf) - 1);
                    // buf[sizeof(buf)-1] = '\0';//string传给buf
                    write(cfd, buf, strlen(buf)); // 关键json串传给服务器
                    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    break;
                }
                cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        bzero(buf, sizeof(buf));
        nByte = read(cfd, buf, sizeof(buf)); // 获取服务器传回的网页集合json串
        string ptmp(buf);                    // buf转为string
        content.clear();
        content = transJson.initJson(200, ptmp); // 获取解包后网页集合map
        for (auto &elem : content)
        { // 输出网页集合
            cout << elem.second << endl;
        }
    }
    close(cfd);
    return 0;
}