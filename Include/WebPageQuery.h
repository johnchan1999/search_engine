#pragma once
#ifndef __ALOPEX_WebPageQuery_H__
#define __ALOPEX_WebPageQuery_H__

#include "WordSegmentation.h"
#include "WebPage.h"
#include <unordered_map>
#include <utility>

using std::multimap;
using std::pair;
using std::unordered_map;

class WebPageQuery
{
public:
    WebPageQuery(Configuration &conf);
    ~WebPageQuery();
    // 参数为用户输入的搜索词或语句，进行查询。根据用户的查询词，分词，存入vector<string> queryWord
    // 然后将分词后的数组传入getQueryWordsWeithtVector函数，计算权重，存入vector<double> 表示基准向量X
    string do_query(const string &str);

    // 将偏移库和倒排索引库文件的内容读进来
    void loadOffsetLib();
    void loadInvertIndex();
    void loadFileOfWordCount();
    void loadLibary();

    // 计算词频存入一个map，根据TF-IDF算法计算每个关键字的权重，存入vector<double> _baseX。即为基准向量
    void getQueryWordsWeithtVector(vector<string> &queryWord);
    // 计算查询词的权重
    void clacWeight();
    // 通过倒排索引表去查找包含所有关键字的网页
    // 只要有关键字在倒排索引表查询不到，returnNoAnswer()
    // 所有关键词都能找到，invertTable[word]是一个vector<pair<int, double>>。则用map<int, int>容器，将所有关键词的网页ID进行统计
    // 遍历每个键值对，如果value = _queryWordFreq.size()   则该网页ID表示交集。取出其对应的权重,即为所有的向量Y
    // 然后计算余弦值
    void keyIsExit();
    double clacCos(vector<double> X, vector<double> Y);

    void executeQuery();

    string createJson();

    string returnNoAnswer();

private:
    Configuration &_conf;
    WordSegmentation _jieba; // 分词对象

    vector<string> _queryWord;                           // 存放用户查询内容分词后的结果
    unordered_map<string, int> _queryWordFreq;           // 查询词的词频map
    vector<double> _baseX;                               // 存放基准向量X
    multimap<double, int, std::greater<double>> _retMap; // 保存要返回网页的余弦值，网页ID

    unordered_map<string, int> _wordFrequency;   // 网页库所有词组的词频
    unordered_map<string, int> _fileOfWordCount; // 包含某个词的文档数量，即DF
    set<string> _stopWords;                      // 存放停用词的集合
    map<string, string> _confMap;                // 存放配置文件的内容

    vector<pair<int, WebPage>> _pageLib;                                // 网页库-->待发送网页的ID 以及 网页对象
    unordered_map<int, pair<int, int>> _offsetLib;                      // 偏移库-->网页ID  起始位置  网页大小
    unordered_map<string, vector<pair<int, double>>> _invertIndexTable; // 倒排索引库
};

#endif
