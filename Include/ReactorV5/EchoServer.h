#ifndef __ECHOSERVER_HH__
#define __ECHOSERVER_HH__

#include <nlohmann/json.hpp>
#include "ThreadPool.h"
#include "TcpServer.h"
#include "TcpConnection.h"
#include "Thread.h"
#include "../KeyRecommander.h"
#include "../WebPageSearch.h"
#include "../LRUcache/CacheManager.h"

#include <iostream>

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
        MyTask(const string &msg, const TcpConnectionPtr &con)
            : _msg(msg), _con(con) {}

        void process1()
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
            CacheManager *pInstance = CacheManager::createInstance();
            auto cache = pInstance->getcache(atoi(current_thread::_name) + 1);

            std::cout << ">> 收到客户端查询词：" << word << std::endl;
            string result = checkLRU(1, word, cache);
            cout << "result==" << result << endl;

            if (" " == result)
            {
                result = getKeyRecommander(word, _con);
                std::cout << ">> 读取磁盘返回关键词查询结果成功 " << std::endl;
                if (storeToLRU(1, word, result, cache) == false)
                {
                    std::cerr << "Error: 53 :storedRedis keyword failed" << result << std::endl;
                }
            }
            else
            {
                std::cout << ">> LRU获取关键词查询结果成功 " << result << std::endl;
            }
            result.push_back('\n');
            _con->sendInLoop(result);
        }
        void process2()
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
            CacheManager *pInstance = CacheManager::createInstance();
            auto cache = pInstance->getcache(atoi(current_thread::_name) + 1);
            std::cout << ">> 收到客户端查询词：" << word << std::endl;
            string result = checkLRU(2, word, cache);

            if (" " == result)
            {
                result = getWebRecommander(word);
                std::cout << ">> 读取磁盘获取网页查询结果成功 " << result << std::endl;
                storeToLRU(2, word, result, cache);
            }
            else
            {
                std::cout << ">> LRU获取网页查询结果成功 " << result << std::endl;
            }

            result.push_back('\n');
            _con->sendInLoop(result);
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

        string checkLRU(int flag, const string &key, LRUCache &cache)
        {

            return cache.getelement(key);
        }

        bool storeToLRU(int flag, const string &key, const string &value, LRUCache &cache)
        {
            cache.addelement(key, value);
            return true;
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

        //_conPool("127.0.0.1", 6379, MAXNUM)
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
            MyTask task(msg, con);
            int id = task.getjsonid(msg);
            cout << "id = " << id << endl;
            if (id == 1)
            {
                cout << "do process id = 1" << endl;
                _pool.addTask(std::bind(&MyTask::process1, task)); // 将任务添加到线程池
            }
            else if (id == 2)
            {
                cout << "do process id = 2" << endl;
                _pool.addTask(std::bind(&MyTask::process2, task)); // 将任务添加到线程池
            }
            else
            {
                std::cerr << "Error: 消息id错误" << id << std::endl;
            } // 创建新的任务
        }

        // 3、连接断开做的事件
        void onClose(const TcpConnectionPtr &con)
        {
            cout << con->toString() << " has closed!" << endl;
        }

    private:
        ThreadPool _pool;
        TcpServer _server;
    };

}; // end of OurPool

#endif