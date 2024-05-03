#pragma once
#ifndef __DirScanner_H__
#define __DirScanner_H__

#include <vector>
#include <string>
using std::string;
using std::vector;

class DirScanner
{
public:
    DirScanner();
    ~DirScanner();
    void operator()();
    // 返回_vecFiles的引用
    vector<string> &getFilePath();
    // 获取某一目录下的所有文件
    void traverse(const string &dirname);

private:
    // 存放每个语料文件的绝对路径
    vector<string> _filePath;
};

#endif
