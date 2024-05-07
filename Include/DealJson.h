#ifndef __DealJson_H__
#define __DealJson_H__

#include <nlohmann/json.hpp>
#include <string>
#include <map>
using std::map;
using std::string;
using json = nlohmann::json;


class DealJson
{
public:
    DealJson();
    ~DealJson();

    map<int,string> & initJson(int id, string word); //初始化json

    void packet(); // 打包，普通字符串--->json字符串
    void unpack(); // 解包，json字符串--->json对象
    int decodeKey(); //id为100，解码关键字
    int decodePage(); //id为200，解码网页

private:
    int _id;
    string _word;
    map<int,string> _result;
    json _jsonObj;    
};

#endif