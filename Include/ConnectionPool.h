#include <hiredis/hiredis.h>
#include <mutex>
#include <vector>
#include <string>
#include <pthread.h>

using std::lock_guard;
using std::mutex;
using std::string;
using std::vector;
#if 0
class redisLock
{
public:
    redisLock()
    : writeLock(PTHREAD_MUTEX_INITIALIZER)
    , readLock(PTHREAD_MUTEX_INITIALIZER)
    {
        pthread_mutex_init(&readLock, NULL);
        pthread_mutex_init(&writeLock, NULL);
    }
    ~redisLock()
    {
        pthread_mutex_destroy(&readLock);
        pthread_mutex_destroy(&writeLock);
    }

    void lockRead()
    {
        pthread_mutex_lock(&writeLock);
    }
    void unLockRead()
    {
        pthread_mutex_unlock(&writeLock);
    }
    void lockwrite()
    {
        pthread_mutex_lock(&readLock);
        pthread_mutex_lock(&writeLock);
    }
    void unlockwrite()
    {
        pthread_mutex_unlock(&readLock);
        pthread_mutex_lock(&writeLock);
    }


private:
    pthread_mutex_t writeLock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t readLock = PTHREAD_MUTEX_INITIALIZER;
};
#endif

class ConnectionPool
{
public:
    ConnectionPool(string host, int port, int maxNum);
    ~ConnectionPool();

    redisContext *getConnection(); // 获取一个连接
#if 0
    redisLock & getRedisLock()
    {
        return _redisLock;
    }
#endif

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