#pragma once
#ifndef __ALOPEX_WebPage_HPP__
#define __ALOPEX_WebPage_HPP__


#include "WebConfiguration.h"
#include "WordSegmentation.h"
#include <string>
#include <vector>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;

// 前向声明
class WordSegmentation;

const static int TOPK_NUMBER = 20;

class WebPage
{
    // 全局函数做友元
    friend bool operator==(const WebPage &lhs, const WebPage &rhs);
    friend bool operator<(const WebPage &lhs, const WebPage &rhs);
public:
    WebPage(string &doc, Configuration &config, WordSegmentation &jieba);
    ~WebPage();
    void    processDoc();
    string  getDoc();
    int     getDocID();
    string  getTitle();
    string  getUrl();
    string  getContent();
    int     getTotalWords();
    void    statisticsWord();
    unordered_map<string, int>& getWordsMap();
    void    calcTopK(int k);
private:
    size_t checkUTF8(const char ch);

private:
    string              _doc;                   // 包括xml在内的整篇文档
    int                 _docID;                 // 文档id
    string              _docTitle;              // 文档标题
    string              _docUrl;                // 文档URL
    string              _docContent;            // 文档内容  (content 标签的内容)
    vector<string>      _topWords;              // 词频最高的前20个词
    unordered_map<string, int>    _wordsMap;    // 每篇文档的词语和词频（不包含停用词）
    Configuration       &_config;               // 配置文件类对象的引用
    WordSegmentation    &_jieba;                // 分词对象
};


#endif

