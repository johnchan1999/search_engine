#include "../../Include/isChinese.h"
#include "../../Include/KeyRecommander.h"
#include <nlohmann/json.hpp>
#include "../../Include/EditDist.h"
#include <unistd.h>
#include <algorithm>
#include <set>

namespace SearchEngine
{
    using OurPool::TcpConnection;
    using std::set;

    KeyRecommander::KeyRecommander(string &query, const TcpConnectionPtr &conn, Dictionary &diction)
        : _conn(conn), _resultQue(MyCompare()), _diction(diction), _queryWord(query), _enIndex(diction.getEnIndexTable()), _cnIndex(diction.getCnIndexTable()), _endict(diction.getenDict()), _cndict(diction.getcnDict())
    {
    }

    // 执行查询
    void KeyRecommander::execute()
    {
        queryIndexTable();
    }

    // 查询索引
    void KeyRecommander::queryIndexTable()
    {
        // 判断输入的是中文还是英文
        if (getByteNum_utf8(_queryWord[0]) == 3)
        {
            queryCnWord(_queryWord);
        }
        else
        {
            queryEnWord(_queryWord);
        }
    }

    void KeyRecommander::queryCnWord(const string &queryWord)
    {
        string str;
        int flag = 0;
        vector<string> sword;
        for (auto i : queryWord)
        {
            str += i;
            if (++flag, flag == 3)
            {
                sword.push_back(str);
                flag = 0;
                str.clear();
            }
        }

        vector<set<int>> temp;
        for (auto &i : sword)
        {
            
            if (_cnIndex.find(i) != _cnIndex.end())
            {   
                temp.push_back(_cnIndex[i]);                
            }
        }

        for (int i = 0; i < temp.size(); ++i)
        {   
            statistic(0, temp[i]);
        }
    }

    void KeyRecommander::queryEnWord(const string &singleWord)
    {
        vector<set<int>> temp;
        for (int i = 0; i < singleWord.size(); ++i)
        {
            string tem(1, singleWord[i]);
            temp.push_back(_enIndex[tem]);
        }
        for (int i = 0; i < temp.size(); ++i)
        {
            statistic(1, temp[i]);
        }
    }

    // 进行计算
    void KeyRecommander::statistic(int flag, set<int> &iset)
    {
        //  从词典类对象获取词典文件内容
        // flag：0中文, 1英文
        if (0 == flag)
        {
            for (auto &tem : iset)
            {
                int i = tem;
                auto &similiar = _cndict[--i];
                MyResult myresult(similiar.first, similiar.second, distance(similiar.first));
                _resultQue.push(myresult);
            }
        }
        else
        {
            for (auto &tem : iset)
            {
                auto &similiar = _endict[tem];
                MyResult myresult(similiar.first, similiar.second, distance(similiar.first));
                _resultQue.push(myresult);
            }
        }
    }

    // 计算最小编辑距离
    int KeyRecommander::distance(const string &rhs)
    {
        return neditDistance(_queryWord, rhs);
    }

    using nlohmann::json;

    // 响应客户端的请求
    string KeyRecommander::response()
    {
        json jsonData;
        string retWord;
        std::cout << ">> Return key word count：" << _resultQue.size() << std::endl;
        if (_resultQue.size() > 8)
        {
            for (int i = 0; i < 8; ++i)
            {
                MyResult result = _resultQue.top();
                _resultQue.pop();
                retWord += result._word + " ";
            }
        }
        else
        {
            int times = _resultQue.size();
            while (times-- > 0)
            {
                MyResult result = _resultQue.top();
                _resultQue.pop();
                retWord += result._word + " ";
            }
        }
        jsonData["id"] = 100;
        jsonData["word"] = retWord;
        jsonData["length"] = retWord.size();
        _queryWord = jsonData.dump();
        return _queryWord;
    }

}; // end of SearchEngine