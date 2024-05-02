#ifndef __ACCEPTOR_H__
#define __ACCEPTOR_H__

#include "Socket.h"
#include "InetAddress.h"
#include <string>

namespace OurPool
{
    using std::string;

    class Acceptor
    {

    public:
        Acceptor(const string &ip, unsigned short port);
        ~Acceptor();
        void setReuseAddr(); // 设置可重用地址
        void setReusePort(); // 设置可重用端口号
        int fd() const;      // 放回套接字fd
        void listen();       // 监听套接字
        int accept();        // 接收新连接，并返回对应fd
        void ready();        // 给传来的IP和端口初始化
        void bind();         // 套接字fd绑定相应端口、地址

    private:
        InetAddress _addr; // 地址类对象
        Socket _sock;      // 套接字对象
    };

}; // end of OurPool

#endif
