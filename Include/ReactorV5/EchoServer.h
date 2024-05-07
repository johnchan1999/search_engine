#ifndef __ECHOSERVER_H__
#define __ECHOSERVER_H__

#include <nlohmann/json.hpp>
#include "ThreadPool.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "../KeyRecommander.h"
#include "../WebPageSearch.h"
#include "../ConnectionPool.h"

#include <iostream>

const int MAXNUM = 20;

namespace OurPool
{
    using nlohmann::json;
    using SearchEngine::Dictionary;
    using SearchEngine::KeyRecommander;
    using std::cout;
    using std::endl;

    class MyTask
    {
    public:
        MyTask(const string &msg, const TcpConnectionPtr &con, ConnectionPool &pool)
            : _msg(msg), _con(con), _pool(pool) {}

        void process()
        {
            if (_msg.size() == 0)
            {
                return;
            }
            string word = getjsonword(_msg);
            int id = getjsonid(_msg);
            if (word.back() == '\n')
            {
                word.pop_back();
            }
            readmsg(id, word);
        }

        void readmsg(int id, string word)
        {
            std::cout << ">> 收到客户端查询词：" << word << std::endl;
            if (1 == id)
            {
                string result = checkRedis(1, word);
                if (" " == result)
                {
                    result = getKeyRecommander(word, _con);
                    std::cout << ">> 读取磁盘返回关键词查询结果成功 " << std::endl;
                    if (storeToRedis(1, word, result) == false)
                    {
                        std::cerr << "Error: 53 :storedRedis keyword failed" << result << std::endl;
                    }
                }
                else
                {
                    std::cout << ">> redis获取关键词查询结果成功 " << result << std::endl;
                }
                result.push_back('\n');
                _con->sendInLoop(result);
            }
            else if (2 == id)
            {
                string result = checkRedis(2, word);

                if (" " == result)
                {
                    result = getWebRecommander(word);
                    std::cout << ">> 读取磁盘获取网页查询结果成功 " << result << std::endl;
                    storeToRedis(2, word, result);
                }
                else
                {
                    std::cout << ">> redis获取网页查询结果成功 " << result << std::endl;
                }

                result.push_back('\n');
                _con->sendInLoop(result);
            }
            else
            {
                std::cerr << "Error: 消息id错误" << id << std::endl;
            }
        }

        string getjsonword(const string &jsondata)
        {
            json jsonjson = json::parse(jsondata);
            string word = jsonjson["word"];
            return word;
        }

        int getjsonid(const string &jsondata)
        {
            json jsonjson = json::parse(jsondata);
            int id = jsonjson["id"];
            return id;
        }

        string checkRedis(int flag, const string &msg)
        {
            redisContext *predis = _pool.getConnection();

            if (predis != nullptr)
            {
                redisReply *reply = (redisReply *)redisCommand(predis, "select %d", flag);
                reply = (redisReply *)redisCommand(predis, "get %s", msg.c_str());
                if (reply == nullptr)
                {
                    _pool.releaseConnection(predis);
                    return " ";
                }
                else
                {
                    if (REDIS_REPLY_STRING == reply->type)
                    {
                        string result = reply->str;
                        freeReplyObject(reply);
                        _pool.releaseConnection(predis);
                        return result;
                    }
                    freeReplyObject(reply);
                    _pool.releaseConnection(predis);
                }
            }
            else
            {
                std::cerr << "checkRedis: redis connection failed" << std::endl;
            }
            return " ";
        }

        bool storeToRedis(int flag, const string &key, const string &value)
        {
            redisContext *predis = _pool.getConnection();
            if (predis != nullptr)
            {
                redisReply *reply = (redisReply *)redisCommand(predis, "select %d", flag);
                reply = (redisReply *)redisCommand(predis, "set %s %s", key.c_str(), value.c_str()); // 命令行参数格式const char*
                if (reply == nullptr)
                {
                    freeReplyObject(reply);
                    _pool.releaseConnection(predis);
                    return false;
                }
                else
                {
                    if (REDIS_REPLY_STATUS == reply->type && 0 == strcmp(reply->str, "OK"))
                    {
                        std::cout << ">> " << key << "查询结果存储至redis" << std::endl;
                        freeReplyObject(reply);
                        _pool.releaseConnection(predis);
                        return true;
                    }
                    freeReplyObject(reply);
                }
                _pool.releaseConnection(predis);
            }
            else
            {
                std::cerr << "storeToRedis: redis connection failed" << std::endl;
            }
            return false;
        }

        string getKeyRecommander(string msg, TcpConnectionPtr &con)
        {
            Dictionary *diction = Dictionary::createInstance();
            diction->init("/root/search_engine/data/datfile");
            KeyRecommander reckey(msg, con, *diction);
            reckey.execute();
            
            return reckey.response();
        }
        string getWebRecommander(const string &msg)
        {
            Configuration *cconf = Configuration::getInstance("/root/search_engine/conf/WebConf.conf");
            WebPageQuery query(*cconf);
            WebPageSearch web(msg);
            return web.doQuery(query);
        }

    private:
        ConnectionPool &_pool;
        TcpConnectionPtr _con; // 用于发送消息
        string _msg;           // 存放发送的消息
    };

    class EchoServer
    {
    public:
        EchoServer(size_t threadNum, size_t queSize, const string &ip, unsigned short port)
            : _pool(threadNum, queSize) // 初始化线程池和任务队列
              ,
              _server(ip, port) // 初始化服务器
              ,
              _conPool("127.0.0.1", 6379, MAXNUM)
        {
        }

        ~EchoServer() {}

        void start()
        {
            _pool.start();
            using namespace std::placeholders;
            _server.setAllCallback(std::bind(&EchoServer::onNewConnection, this, _1) // 新连接时调用的回调函数
                                   ,
                                   std::bind(&EchoServer::onMessage, this, _1) // 新消息到来时的回调函数
                                   ,
                                   std::bind(&EchoServer::onClose, this, _1)); // 旧连接断开的回调函数
            _server.start();
        }

        void stop()
        {
            _pool.stop();
            _server.stop();
        }

    private:
        // 1、连接建立做的事件
        void onNewConnection(const TcpConnectionPtr &con)
        {
            cout << con->toString() << " has connected!" << endl;
        }

        // 2、消息到达做的事件
        void onMessage(const TcpConnectionPtr &con)
        {
            string msg = con->receive(); // 接收客户端的数据
            cout << ">>recv msg from client " << msg << endl;
            MyTask task(msg, con, _conPool);                  // 创建新的任务
            _pool.addTask(std::bind(&MyTask::process, task)); // 将任务添加到线程池
        }

        // 3、连接断开做的事件
        void onClose(const TcpConnectionPtr &con)
        {
            cout << con->toString() << " has closed!" << endl;
        }

    private:
        ThreadPool _pool;
        TcpServer _server;
        ConnectionPool _conPool;
    };

}; // end of OurPool

#endif
