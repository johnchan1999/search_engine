#ifndef __MYRESULT_H__
#define __MYRESULT_H__

#include <string>

struct MyResult
{
    MyResult(std::string &word, int freq, int dist)
        : _word(word) // 单词
          ,
          _freq(freq) // 词频
          ,
          _dist(dist) // 最小编辑距离
    {
    }

    ~MyResult()
    {
    }

    std::string _word;
    int _freq;
    int _dist;
};

struct MyCompare
{
    bool operator()(const MyResult &lhs, const MyResult &rhs)
    {
        if (lhs._dist > rhs._dist)
        {
            return true;
        }
        else if (lhs._dist == rhs._dist)
        {
            if (lhs._freq < rhs._freq)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        return false;
    }
};

#endif