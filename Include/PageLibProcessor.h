#pragma once
#ifndef __PageLibProcessor_H__
#define __PageLibProcessor_H__

#include "WordSegmentation.h"
#include "WebConfiguration.h"
#include "WebPage.h"
#include <vector>
#include <utility>

using std::vector;
using std::pair;


class PageLibProcessor
{
public:
    PageLibProcessor(Configuration &conf);
    ~PageLibProcessor();
    void doProcess();
private:
    void readInfoFromFile();    //根据配置信息读取网页库和位置偏移库的内容
    void cutRedundantPages();   //对冗余的网页进行去重
    void buildInvertIndexTable();   //创建倒排索引表
    void storeFileOfWordCount();
    void storeOnDisk();     //将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上

private:
    string _tmpPagePath;
    string _tmpOffsetPath;
    string _pageLibPath;
    string _offsetLibPath;
    string _invertIndexPath;
    string _fileOfWordCountPath;    // 某个单词出现的文档数 保存路径
    Configuration &_conf;
    WordSegmentation _jieba;    // 分词对象
    vector<WebPage> _pageLib;   //网页库的容器对象
    unordered_map<int, pair<int, int> > _offsetLib;     //网页偏移库对象
    unordered_map<string, vector<pair<int, double>>> _invertIndexTable;     //倒排索引表对象
    unordered_map<string, int> _fileOfWordCountMap;
};


#endif

