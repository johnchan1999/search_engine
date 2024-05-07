#include <hiredis/hiredis.h>
#include <mutex>
#include <vector>
#include <string>
#include <pthread.h>

using std::lock_guard;
using std::mutex;
using std::string;
using std::vector;


class ConnectionPool
{
public:
    ConnectionPool(string host, int port, int maxNum);
    ~ConnectionPool();

    redisContext *getConnection(); // 获取一个连接

    void releaseConnection(redisContext *connection); // 释放连接

private:
    redisContext *createConnection(); // 创建连接

    bool isConnection(redisContext *connection); // 查看连接状态

private:
    string _host;                        // ip地址
    int _port;                           // 端口号
    int _maxNum;                         // 最大连接数
    vector<redisContext *> _connections; // 连接队列
    mutex _mutex;                        // 锁
    // redisLock   _redisLock;
};