#include "../Include/DealJson.h"
#include <sstream>

using std::istringstream;

DealJson::DealJson()
{
}

DealJson::~DealJson()
{
}

map<int, string> &DealJson::initJson(int id, string word)
{
    _result.clear(); // 清空map容器

    _id = id;
    _word = word;

    if (_id == 1 || _id == 2)
    {
        packet();
        return _result;
    }

    unpack();
    return _result;
}

// 返回json 格式的字符串
void DealJson::packet()
{
    _jsonObj["id"] = _id;
    _jsonObj["word"] = _word;            // 此时word代表关键词字符串   或  网页嵌套json包字符串
    _jsonObj["length"] = _word.length(); // 待查询内容的长度

    _result[1] = _jsonObj.dump();
    
}

// json字符串解析
void DealJson::unpack()
{
    _jsonObj = json::parse(_word);
    _id = _jsonObj["id"];

    if (_id == 100)
    {
        decodeKey();
    }
    else
    {
        decodePage();
    }
}

int DealJson::decodeKey()
{
    _word = _jsonObj["word"];
    if (_word.length() != _jsonObj["length"])
    {
        _result[-1] = "关键词集合传输错误";
        return 1;
    }

    if (_word == " ")
    { // 关键词未命中 返回空格
        _result[0] = "未匹配到相关关键词";
        return 1;
    }

    istringstream iss(_word); // 传回的关键词集合以空格分隔
    string key;
    int num = 1;
    while (iss >> key)
    {
        _result[num++] = key;
    }
    return 1;
}

int DealJson::decodePage()
{
    _word = _jsonObj["word"];
    if (_word.length() != _jsonObj["length"])
    {
        _result[-1] = "网页集合传输错误";
        return 1;
    }

    if (_word == " ")
    { // 关键词未命中 返回空格
        _result[0] = "未匹配到相关网页";
        return 1;
    }

    json page = json::parse(_word); //_word中是网页查询json的字符串
    int num = 1;
    for (auto &elem : page["WebPages"])
    {
        string title;
        if (elem["title"].is_null())
        {
            title = string();
        }
        else
        {
            title = elem["title"];
        }
        _result[num++] = "title   = " + title;

        string url;
        if (elem["url"].is_null())
        {
            url = string();
        }
        else
        {
            url = elem["url"];
        }
        _result[num++] = "url   = " + url;

        string summary;
        if (elem["summary"].is_null())
        {
            summary = string();
        }
        else
        {
            summary = elem["summary"];
        }
        _result[num++] = "summary = " + summary;
        _result[num++] = " ";
    }
    return 1;
}