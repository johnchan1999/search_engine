#include "../../Include/ReactorV5/EventLoop.h"
#include "../../Include/ReactorV5/TcpConnection.h"
#include "../../Include/ReactorV5/Acceptor.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <sys/eventfd.h>
#include <iostream>
#include <utility>

namespace OurPool
{
    using std::cerr;
    using std::cout;
    using std::endl;

    EventLoop::EventLoop(Acceptor &acceptor)
        : _epfd(createEpollFd()), _evfd(createEventFd()), _isLooping(false), _acceptor(acceptor), _evtList(1024), _mutex()
    {
        // 将listenfd放在红黑树上进行监听
        addEpollReadFd(_acceptor.fd());
        // 将_evfd放在红黑树上进行监听
        addEpollReadFd(_evfd);
    }

    EventLoop::~EventLoop()
    {
        close(_epfd);
        close(_evfd);
    }

    // 是否循环
    void EventLoop::loop()
    {
        _isLooping = true;
        while (_isLooping)
        {
            waitEpollFd();
        }
    }

    void EventLoop::unloop()
    {
        _isLooping = false;
    }

    void EventLoop::runInLoop(Functors &&cb)
    {
        // 将要执行的"任务"先存放在vector中
        //"任务"就是：线程池处理好之后的msg
        // 以及发送msg的能力的TcpConnection
        // 中的send函数
        // 锁的粒度（锁的范围）
        {
            MutexLockGuard autoLock(_mutex);
            _pendings.push_back(std::move(cb));
        }

        // 此时需要执行wakeup????
        wakeup();
    }

    void EventLoop::setNewConnectionCallback(TcpConnectionCallback &&cb)
    {
        _onConnectionCb = std::move(cb);
    }

    void EventLoop::setMessageCallback(TcpConnectionCallback &&cb)
    {
        _onMessageCb = std::move(cb);
    }

    void EventLoop::setCloseCallback(TcpConnectionCallback &&cb)
    {
        _onCloseCb = std::move(cb);
    }

    // 执行epoll_wait的函数
    void EventLoop::waitEpollFd()
    {
        int nready;
        do
        {
            nready = epoll_wait(_epfd, &*_evtList.begin(), _evtList.size(), -1);
        } while (-1 == nready && errno == EINTR);

        if (-1 == nready)
        {
            cerr << "-1 == nready " << endl;
            return;
        }
        else if (0 == nready)
        {
            cout << ">>epoll_wait timeout" << endl;
        }
        else
        {
            // 考虑扩容问题
            if (nready == (int)_evtList.size())
            {
                _evtList.resize(2 * nready);
            }

            for (int idx = 0; idx < nready; ++idx)
            {
                int fd = _evtList[idx].data.fd;
                // 新的连接
                if (fd == _acceptor.fd()) //_acceptor.fd()就是listenfd
                {
                    if (_evtList[idx].events & EPOLLIN)
                    {
                        handleNewConnection();
                    }
                }
                else if (fd == _evfd)
                {
                    if (_evtList[idx].events & EPOLLIN)
                    {
                        handleRead();
                        // 将所有的"任务"全部都执行一下
                        // 线程池处理好之后的msg以及发送数据
                        // 能力的TcpConnection中的send
                        doPendingFunctors();
                    }
                }
                else // 老的连接
                {
                    if (_evtList[idx].events & EPOLLIN)
                    {
                        handleMessage(fd);
                    }
                }
            }
        }
    }
    // 处理新的连接
    void EventLoop::handleNewConnection()
    {
        // 执行accept函数
        int connfd = _acceptor.accept();
        if (connfd < 0)
        {
            perror("handleNewConnection");
            return;
        }
        // 将accept函数返回的文件描述符放在红黑树上进行监听
        addEpollReadFd(connfd);

        // 创建TcpConnection
        TcpConnectionPtr con(new TcpConnection(connfd, this));

        // 回调机制
        // 回调函数的注册
        con->setNewConnectionCallback(_onConnectionCb); // 1、连接建立
        con->setMessageCallback(_onMessageCb);          // 2、消息到达
        con->setCloseCallback(_onCloseCb);              // 3、连接断开

        // 将文件描述符与TcpConnection以键值对存放在map中
        _conns.insert(std::make_pair(connfd, con));

        // 1、连接已经建立好了，就可以执行连接的建立事件
        con->handleNewConnectionCallback();
    }

    // 处理消息的发送
    void EventLoop::handleMessage(int fd)
    {
        auto it = _conns.find(fd);
        if (it != _conns.end())
        {
            bool flag = it->second->isClosed();
            if (flag)
            {
                // 3、连接断开的事件回调
                it->second->handleCloseCallback();
                // 连接需要从红黑树上删除
                delEpollReadFd(fd);
                // 将文件描述符从map中删除
                _conns.erase(it);
            }
            else
            {
                // 2、消息到达的事件回调
                it->second->handleMessageCallback();
            }
        }
        else
        {
            cout << "该连接不存在" << endl;
        }
    }

    // 创建文件描述符epfd
    int EventLoop::createEpollFd()
    {
        int fd = epoll_create(1);
        if (fd < 0)
        {
            perror("epoll_create");
            return fd;
        }
        return fd;
    }

    // 将文件描述符放在红黑树上进行监听
    void EventLoop::addEpollReadFd(int fd)
    {
        struct epoll_event evt;
        evt.data.fd = fd;
        evt.events = EPOLLIN;

        int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &evt);
        if (ret)
        {
            perror("addEpollReadFd");
            return;
        }
    }

    // 将文件描述符从红黑树上删除
    void EventLoop::delEpollReadFd(int fd)
    {
        struct epoll_event evt;
        evt.data.fd = fd;
        evt.events = EPOLLIN;

        int ret = epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &evt);
        if (ret)
        {
            perror("delEpollReadFd");
            return;
        }
    }
    void EventLoop::handleRead()
    {
        uint64_t one = 1;
        ssize_t ret = read(_evfd, &one, sizeof(uint64_t));
        if (ret != sizeof(uint64_t))
        {
            perror("read");
            return;
        }
    }
    void EventLoop::wakeup()
    {
        uint64_t one = 1;
        ssize_t ret = write(_evfd, &one, sizeof(uint64_t));
        if (ret != sizeof(uint64_t))
        {
            perror("write");
            return;
        }
    }
    int EventLoop::createEventFd()
    {
        int fd = eventfd(10, 0);
        if (fd < 0)
        {
            perror("eventfd");
            return fd;
        }
        return fd;
    }
    void EventLoop::doPendingFunctors() // 将vector中的所有回调函数进行执行
    {
        vector<Functors> tmp;
        // 将vector中存在的cb取出来，放在另外的容器中进行遍历
        // 那么原来的vector就可以释放出来，可以往其中写数据
        {
            MutexLockGuard autoLock(_mutex);
            tmp.swap(_pendings);
        }
        // 遍历vector，将所有的cb进行执行
        for (auto &cb : tmp)
        {
            cb();
        }
    }
}; // end of OurPool
