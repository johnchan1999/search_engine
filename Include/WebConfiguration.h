#pragma once
#ifndef __WEBConfiguration_H__
#define __WEBConfiguration_H__

#include <map>
#include <set>
#include <string>
using std::map;
using std::set;
using std::string;

class Configuration
{
public:
    static Configuration *getInstance(const string &configFilePath);
    static void destory();
    // 获取存放配置文件内容的map
    map<string, string> &getConfigMap();
    // 获取停用词词集
    set<string> &getStopWordList();

private:
    Configuration(const string &configFilePath);
    ~Configuration();
    void getStopEngword();
    void getStopZhword();

private:
    // 静态成员变量
    static Configuration *_pInstance;
    // 配置文件路径
    string _configFilePath;
    // 配置文件内容 <配置名称，实际参数>
    map<string, string> _configMap;
    // 停用词词集
    set<string> _stopWordList;
};

#endif
