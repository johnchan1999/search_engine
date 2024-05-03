#pragma once
#ifndef __ALOPEX_PageLib_HPP__
#define __ALOPEX_PageLib_HPP__

#include "DirScanner.h"
#include "WebConfiguration.h"
#include "FileProcessor.h"
#include <map>
#include <utility>
using std::pair;
using std::map;


class PageLib
{
public:
    PageLib(Configuration &conf, DirScanner &dirScanner, FileProcessor &fileProcess);
    ~PageLib();
    // 创建网页库
    void createPageLib();
    // 创建位置偏移库
    void createOffsetLib();

    // 创建网页库
    //void creat();
    // 存储网页库和位置偏移库
    //void store();
private:
    // 保存网页库
    void storePageLib();
    // 保存位置偏移库
    void storeOffsetLib();
    
     // 检查目录是否存在                                                                                                                     
     bool isDirectoryExists(const string& directory);
     // 创建目录
     bool createDirectory(const string& directory); 
     // 创建目录并打开文件
     ofstream openFile(const string& filePath);

private:
    Configuration &_conf;
    DirScanner &_dirScanner;
    FileProcessor &_process;
    // 存放格式化之后的网页(网页库)
    vector<RssItem> _pageLib;
    // 存放每篇文档在网页库的位置信息（位置偏移库）
    map<int, pair<int, int>> _offsetLib;
};


#endif

