#ifndef __KEYRECOMMANDER_HPP__
#define __KEYRECOMMANDER_HPP__

#include "ReactorV5/TcpConnection.h"
#include "Dictionary.h"
#include "MyResult.h"
#include <string>
#include <queue>
#include <set>
#include <vector>

namespace SearchEngine
{
    using OurPool::TcpConnection;
    using std::priority_queue;
    using std::set;
    using std::shared_ptr;
    using std::string;
    using std::vector;

    class KeyRecommander
    {
        using TcpConnectionPtr = shared_ptr<TcpConnection>;

    public:
        KeyRecommander(string &, const TcpConnectionPtr &, Dictionary &);
        int distance(const string &);     // 计算最小编辑距离
        void statistic(int, set<int> &);  // 进行计算
        void queryIndexTable();           // 查询索引
        void queryCnWord(const string &); // 查询中文词
        void queryEnWord(const string &); // 查询英文词
        string response();                // 响应客户端的请求
        void execute();                   // 执行查询

    private:
        priority_queue<MyResult, vector<MyResult>, MyCompare> _resultQue; // 保存候选结果集的优先级队列
        vector<std::pair<string, int>> _endict;                           // 存放英文词典：短语 + 词频
        vector<std::pair<string, int>> _cndict;                           // 存放中文词典：短语 + 词频
        map<string, set<int>> _enIndex;                                   // 存放英文索引：短语 + 词典索引
        map<string, set<int>> _cnIndex;                                   // 存放中文索引：短语 + 词典索引
        TcpConnectionPtr _conn;                                           // 对应的客户端连接
        Dictionary &_diction;                                             // 词典类对象
        string _queryWord;                                                // 等待查询的单词
    };

}; // end of SearchEngine

#endif