#include "../../Include/WebConfiguration.h"
#include <iostream>
#include <fstream>
#include <sstream>

using std::cerr;
using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;

// 静态成员变量类内声明，类外初始化
Configuration *Configuration::_pInstance = nullptr;

Configuration::Configuration(const string &configFilePath)
    : _configFilePath(configFilePath) {}

Configuration::~Configuration() {}

Configuration *Configuration::getInstance(const string &configFilePath)
{
    if (nullptr == _pInstance)
    {
        _pInstance = new Configuration(configFilePath);
    }
    return _pInstance;
}

void Configuration::destory()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

// 获取存放配置文件内容的map
map<string, string> &Configuration::getConfigMap()
{
    ifstream ifs(_configFilePath);
    if (!ifs)
    {
        cerr << "ifs open " << _configFilePath << "error" << endl;
        exit(1);
    }

    string line;
    string first, second;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        iss >> first >> second;
        _configMap[first] = second;
    }
    return _configMap;
}

// 获取停用词词集
set<string> &Configuration::getStopWordList()
{
    getConfigMap();
    getStopEngword();
    getStopZhword();
    return _stopWordList;
}

// 先获取英文停用词
void Configuration::getStopEngword()
{
    auto it = _configMap.find("stopEng");
    string stopEngPath;
    if (it != _configMap.end())
    {
        stopEngPath = it->second;
    }

    ifstream ifs(stopEngPath);
    if (!ifs)
    {
        cerr << "ifs open " << stopEngPath << " error " << endl;
        exit(1);
    }

    string word;
    while (getline(ifs, word))
    {
        _stopWordList.insert(word);
    }
}

// 再获取中文停用词
void Configuration::getStopZhword()
{
    auto it = _configMap.find("stopZh");
    string stopZhPath;
    if (it != _configMap.end())
    {
        stopZhPath = it->second;
    }

    ifstream ifs(stopZhPath);
    if (!ifs)
    {
        cerr << "ifs open " << stopZhPath << " error " << endl;
        exit(1);
    }

    string word;
    while (getline(ifs, word))
    {
        _stopWordList.insert(word);
    }
}
