#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__

#include "Acceptor.h"
#include "EventLoop.h"

namespace OurPool
{

class TcpServer
{
public:
    TcpServer(const string &ip, unsigned short port);
    ~TcpServer();

    // 让服务器运行起来
    void start();

    // 让服务器停止
    void stop();

    void setAllCallback(TcpConnectionCallback &&onConnection
                    , TcpConnectionCallback &&onMessage
                    , TcpConnectionCallback &&onClose);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};

}; // end of OurPool

#endif
