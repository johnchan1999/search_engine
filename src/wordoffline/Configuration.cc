#include "../../Include/Configuration.h"
#include <fstream>
#include <string.h>
#include <vector>
#include <iostream>
#include <unistd.h>

namespace SearchEngine
{
    
Configuration::Configuration(const string filepath)
: _filepath(filepath)
, _log()
{   }

// 获取存放配置文件内容的 map
map<string, string> &Configuration::getConfigMap()
{
    std::vector<string> feature =
        {"chineseword", "englishword", "stopwordsch", "stopwordsen"};

    std::ifstream input;
    input.open(_filepath, std::ios_base::in);
    if(!input.is_open())
    {
        printf("Error: open %s failed", _filepath.c_str());
        _log.error(LOGPATTERN(__FILE__, __LINE__));
        exit(1);
    }
    string line;
    while(std::getline(input, line))
    {
        string comWord = line.substr(0, 11);
        for(int n = 0; n < feature.size(); ++n)
        {
            if(comWord.compare(feature[n]) == 0)
            {
                string str;
                int flag = 0;
                for(auto i : line)        
                {
                    if('"' == i && 0 == flag)
                    {
                        flag = 1;
                        continue;
                    }
                    else if('"' == i && 1 == flag)
                    {
                        break;
                    }

                    if(1 == flag)
                    {
                        str.push_back(i);
                    }
                }
                _configMap.insert({feature[n], str});
            }
        }
    }
    if(_configMap.size() == 0)
    {
        printf("Error: parse %s failed", _filepath.c_str());
        _log.error(LOGPATTERN(__FILE__, __LINE__));
        exit(1);
    }
    return _configMap;
}

// 获取停用词词集 private : string filepath_;`
set<string> Configuration::getEnStopWordList()
{
    auto teEn = _configMap.find("stopwordsen");

    if (teEn == _configMap.end())
    {
        printf("Error: stopwordsen not exist");
        _log.error(LOGPATTERN(__FILE__, __LINE__));
        exit(1);
    }
    
    std::ifstream inputEn(teEn->second);

    if (!inputEn.is_open())
    {
        printf("Error: open %s failed", teEn->second.c_str());
        _log.error(LOGPATTERN(__FILE__, __LINE__));
        exit(1);
    }
    else
    {
        string line;
        while (std::getline(inputEn, line))
        {
            _enStopWordList.insert(line);
        }
        inputEn.close();
    }
    return _enStopWordList;
}

set<string> Configuration::getCnStopWordList()
{
    auto teCn = _configMap.find("stopwordsch");
    if (teCn == _configMap.end())
    {
        std::cerr << "stopwordsch not exist" << std::endl;
        _log.error(LOGPATTERN(__FILE__, __LINE__));
        exit(1);
    }
    std::ifstream inputCn(teCn->second);

    if (!inputCn.is_open())
    {
        printf("Error: open %s failed", teCn->second.c_str());
        _log.error(LOGPATTERN(__FILE__, __LINE__));
        exit(1);
    }
    else
    {
        string line;
        while (std::getline(inputCn, line))
        {
            _cnStopWordList.insert(line);
        }
        inputCn.close();
    }
    return _cnStopWordList;
}

}; // end of SearchEngine
