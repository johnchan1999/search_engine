#include "../../Include/ReactorV5/TcpConnection.h"
#include "../../Include/ReactorV5/EventLoop.h"
#include <iostream>
#include <sstream>

namespace OurPool
{
    using std::cout;
    using std::endl;
    using std::ostringstream;

    TcpConnection::TcpConnection(int fd, EventLoop *loop)
        : _loop(loop), _sockIO(fd), _sock(fd), _localAddr(getLocalAddr()), _peerAddr(getPeerAddr())
    {
    }

    TcpConnection::~TcpConnection()
    {
    }

    void TcpConnection::send(const string &msg)
    {
        _sockIO.writen(msg.c_str(), msg.size());
    }

    // 将msg发送给EventLoop，然后让EventLoop将数据发送给客户端
    // 因为EventLoop本身没有发送数据的能力，所以在此处需要将msg
    // 以及发送msg的能力TcpConnection中的send都发送给EventLoop
    void TcpConnection::sendInLoop(const string &msg)
    {
        if (_loop)
        {
            // void runInLoop(function<void()> &&)
            _loop->runInLoop(std::bind(&TcpConnection::send, this, msg));
        }
    }

    string TcpConnection::receive()
    {
        char buff[65535] = {0};
        _sockIO.readLine(buff, sizeof(buff));

        return string(buff);
    }

    string TcpConnection::toString()
    {
        ostringstream oss;
        oss << _localAddr.ip() << ":"
            << _localAddr.port() << "---->"
            << _peerAddr.ip() << ":"
            << _peerAddr.port();

        return oss.str();
    }

    // 判断连接是否断开了
    bool TcpConnection::isClosed() const
    {
        char buf[10] = {0};
        int ret = recv(_sock.fd(), buf, sizeof(buf), MSG_PEEK);

        return (0 == ret);
    }
    void TcpConnection::setNewConnectionCallback(const TcpConnectionCallback &cb)
    {
        _onConnectionCb = cb;
    }

    void TcpConnection::setMessageCallback(const TcpConnectionCallback &cb)
    {
        _onMessageCb = cb;
    }

    void TcpConnection::setCloseCallback(const TcpConnectionCallback &cb)
    {
        _onCloseCb = cb;
    }

    // 执行对应的三个回调函数
    void TcpConnection::handleNewConnectionCallback()
    {
        if (_onConnectionCb)
        {
            /* _onConnectionCb(shared_ptr<TcpConnection>(this)); */
            // 防止智能指针的误用
            _onConnectionCb(shared_from_this());
        }
    }

    void TcpConnection::handleMessageCallback()
    {
        if (_onMessageCb)
        {
            _onMessageCb(shared_from_this());
        }
    }

    void TcpConnection::handleCloseCallback()
    {
        if (_onCloseCb)
        {
            _onCloseCb(shared_from_this());
        }
    }
    // 获取本端的网络地址信息
    InetAddress TcpConnection::getLocalAddr()
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(struct sockaddr);
        // 获取本端地址的函数getsockname
        int ret = getsockname(_sock.fd(), (struct sockaddr *)&addr, &len);
        if (-1 == ret)
        {
            perror("getsockname");
        }

        return InetAddress(addr);
    }

    // 获取对端的网络地址信息
    InetAddress TcpConnection::getPeerAddr()
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(struct sockaddr);
        // 获取对端地址的函数getpeername
        int ret = getpeername(_sock.fd(), (struct sockaddr *)&addr, &len);
        if (-1 == ret)
        {
            perror("getpeername");
        }

        return InetAddress(addr);
    }
}; // end of OurPool
