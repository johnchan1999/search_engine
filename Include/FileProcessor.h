#pragma once
#ifndef __FileProcessor_H__
#define __FileProcessor_H__

#include <string>
#include <fstream>
#include <vector>
using std::ofstream;
using std::string;
using std::vector;

// 接口
struct RssItem
{
    string title;
    string link;
    string description;
    string content;
};

// 处理每一个xml文档
class FileProcessor
{
public:
    FileProcessor() {}
    ~FileProcessor() {}

    // 解析xml文件，存入vector
    void parseRss(const char *filename, vector<RssItem> &_rss);

private:
};

#endif
