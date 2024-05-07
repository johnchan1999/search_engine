#include "../../Include/WebPage.h"
#include <iostream>
#include <sstream>
#include <utility>
#include <regex>
#include <queue>
#include <algorithm>

using std::cout;
using std::endl;
using std::istringstream;
using std::pair;
using std::priority_queue;
using std::regex;
using std::smatch;



WebPage::WebPage(string &doc, Configuration &config, WordSegmentation &jieba)
    : _doc(doc), _config(config), _jieba(jieba)
{
    processDoc();
    statisticsWord();
    calculateSimHash();
}

WebPage::~WebPage() {



}


// 获取文档
string WebPage::getDoc()
{
    return _doc;
}

// 获取文档id
int WebPage::getDocID()
{
    return _docID;
}
// 获取title
string WebPage::getTitle()
{
    return _docTitle;
}
// 过去url
string WebPage::getUrl()
{
    return _docUrl;
}

string WebPage::getContent()
{
    return _docContent;
}

// 获取一篇网页的单词总数
int WebPage::getTotalWords()
{
    int count = 0;
    for (auto elem : _wordsMap)
    {
        count += elem.second;
    }
    return count;
}

// 获取文档的词频统计map
unordered_map<string, int> &WebPage::getWordsMap()
{
    return _wordsMap;
}

// 从xml标签提取信息
string getTagValue(const string &xml, const string &tag)
{
    string pattern = "<" + tag + ">(.*?)</" + tag + ">";
    regex regex(pattern);
    smatch matches;
    regex_search(xml, matches, regex);

    if (matches.size() > 1)
    {
        return matches.str(1);
    }

    return "";
}

size_t WebPage::checkUTF8(const char ch)
{
    int num = 0; // 记录字节数

    for (int i = 0; i < 6; i++)
    {
        if (ch & (1 << (7 - i)))
        {
            num++;
        }
        else
        {
            break; // 断开不连续就退出
        }
    }
    if (num == 3)
    {
        return 3;
    }
    return 1;
}

// 对格式化文档进行处理
// 每篇网页的内容保存在_doc中，对其进行处理
// 获取docID, docTitle, docUrl, docContent, docSummay
void WebPage::processDoc()
{
    _docID = stoi(getTagValue(_doc, "docid"));
    _docUrl = getTagValue(_doc, "url");
    _docTitle = getTagValue(_doc, "title");
    _docContent = getTagValue(_doc, "content");
}

// 统计单词和词频
// 先分词
void WebPage::statisticsWord()
{
    set<string> _stopWordList = _config.getStopWordList();
    string newDoc = std::regex_replace(_doc, std::regex("<.*?>"), ""); // 去除一篇网页的标签
    vector<string> data = _jieba(newDoc);                              // 分词结果保存到vector容器

    for (auto it = data.begin(); it != data.end(); it++)
    {
        if (_stopWordList.count(*it) || *it == " ")
        {
            continue;
        }
        _wordsMap[*it]++;
    }
}

class MyCompare
{
public:
    // 重载函数调用运算符
    // pair<string, int> 第一个类型表示元素，第二个类型表示出现的频率
    bool operator()(pair<string, int> lhs, pair<string, int> rhs)
    {
        // 对于优先级队列，大于符号创建的是小根堆
        // 这和快排的比较函数相反，快排的比较函数大于符号是按照冲大到小排序
        return lhs.second > rhs.second;
    }
};

void WebPage::calculateSimHash()
{
    Simhasher simhasher("/root/search_engine/data/dict/jieba.dict.utf8", "/root/search_engine/data/dict/hmm_model.utf8", "/root/search_engine/data/dict/idf.utf8", "/root/search_engine/data/dict/stop_words.utf8");
    string s(_docContent);
    size_t topN = 5;
    uint64_t u64 = 0;
    vector<pair<string, double>> res;
    simhasher.extract(s, res, topN);
    simhasher.make(s, topN, u64);

    _simhash = u64;
}

// 这篇文档统计完词频后再求TopK
// 求取文档的topK词集
// void WebPage::calcTopK(int k)
// {
//     priority_queue<pair<string, int>, vector<pair<string, int>>, MyCompare> myQue;
//     for (auto elem : _wordsMap)
//     {
//         myQue.push(elem);
//         if ((int)myQue.size() > k)
//         {
//             myQue.pop();
//         }
//     }

//     for (int i = 0; i < k; ++i)
//     {
//         // MyQue的每一个元素都是一个pair对
//         _topWords.push_back(myQue.top().first);
//         myQue.pop();
//     }

//     // 建立的是小根堆，堆顶元素的出现频率是最小的，放到数组的最后面
//     // 将数组元素做个交换即可
//     reverse(_topWords.begin(), _topWords.end());
// }

// 全局函数做友元
// 判断两篇文档是否相等
bool operator==(const WebPage &lhs, const WebPage &rhs)
{
    uint64_t vec1 = lhs._simhash;
    uint64_t vec2 = rhs._simhash;

    bool ret = Simhasher::isEqual(vec1, vec2);
    //cout << "ret = " << ret << "\n";
    return ret;
}

// 对文档按DocID进行排序
bool operator<(const WebPage &lhs, const WebPage &rhs)
{
    return lhs._docID < rhs._docID;
}
