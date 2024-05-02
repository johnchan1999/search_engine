#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include "Log4cpp.h"
#include <set>
#include <string>
#include <map>

namespace SearchEngine
{
    using std::map;
    using std::set;
    using std::string;

    class Configuration
    {
    public:
        Configuration(const string filepath);
        map<string, string> &getConfigMap(); // 获取存放配置文件内容的 map
        set<string> getEnStopWordList();     // 获取停用词词集 private : string filepath_;
        set<string> getCnStopWordList();     // 获取停用词词集 private : string filepath_;

    private:
        map<string, string> _configMap; // 配置文件内容
        set<string> _enStopWordList;    // 停用词词集
        set<string> _cnStopWordList;    // 停用词词集
        string _filepath;               // 配置文件路径
        MyLog _log;                     // 记录日志
    };

};

#endif