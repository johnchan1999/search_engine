#include "../Include/ConnectionPool.h"
#include <iostream>
#include <cstring>

using std::cout;
using std::endl;

ConnectionPool::ConnectionPool(string host, int port, int maxNum)
    : _host(host), _port(port), _maxNum(maxNum)
{
    _connections.reserve(_maxNum);
}

ConnectionPool::~ConnectionPool()
{
    for (auto &elem : _connections)
    {
        redisFree(elem);
    }
}

redisContext *ConnectionPool::getConnection()
{
    lock_guard<mutex> lock(_mutex); // 对该作用于加锁

    if (_connections.empty())
    {
        return createConnection();
    }
    else
    {
        redisContext *connection = _connections.back();
        _connections.pop_back();

        if (!isConnection(connection))
        {
            redisFree(connection);
            return createConnection();
        }
        return connection;
    }
} // 获取一个连接

void ConnectionPool::releaseConnection(redisContext *connection)
{
    lock_guard<mutex> lock(_mutex); // 对该作用于加锁

    _connections.emplace_back(connection);
} // 释放连接

redisContext *ConnectionPool::createConnection()
{
    redisContext *connection = redisConnect(_host.c_str(), _port);
    if (connection == nullptr || connection->err)
    {
        if (connection)
        {
            cout << "redisConnect error" << endl;
            redisFree(connection);
        }
        return nullptr;
    }
    return connection;
} // 创建连接

bool ConnectionPool::isConnection(redisContext *connection)
{
    redisReply *reply = (redisReply *)redisCommand(connection, "PING");
    if (reply == nullptr)
    {
        return false;
    }

    bool status = (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "PONG") == 0);
    freeReplyObject(reply);
    return status;
} // 检查连接