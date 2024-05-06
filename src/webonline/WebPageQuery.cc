#include "../../Include/WebPageQuery.h"
#include <nlohmann/json.hpp>
#include "../../Include/Summary.h"
#include <fstream>
#include <iostream>
#include <sstream>

using std::cerr;
using std::ifstream;
using std::istringstream;
using std::make_pair;
using std::string;
using std::stringstream;

using json = nlohmann::json;

WebPageQuery::WebPageQuery(Configuration &conf)
    : _conf(conf), _jieba()
{
    _stopWords = _conf.getStopWordList(); // 创建WebQuery对象的时候就获取停用词
    _confMap = _conf.getConfigMap();      // 将配置文件的内容存入MAP
}

WebPageQuery::~WebPageQuery()
{
}

string WebPageQuery::do_query(const string &str)
{
    // 加载需要用的库
    loadLibary();
    // 对查询词分词，存入vector
    _queryWord = _jieba(str);

    // 去英文停用词会忽略空格，需要另外判断
    for (auto it = _queryWord.begin(); it != _queryWord.end();)
    {
        if (_stopWords.count(*it) || *it == " ")
        {
            _queryWord.erase(it);
        }
        else
        {
            it++;
        }
    }
    // 输出分词后的结果
    cout << ">> 分词结果: ";
    for (auto &elem : _queryWord)
    {
        cout << elem << " ";
    }
    cout << endl;
    // 基准向量X
    getQueryWordsWeithtVector(_queryWord);
    // 查找交集网页，计算余弦值
    keyIsExit();
    // 执行查询
    executeQuery();

    /* string s = createJson(); */
    /* return s; */
    if (0 == _pageLib.size())
    {
        return returnNoAnswer();
    }
    else
    {
        return createJson();
    }
}

void WebPageQuery::loadLibary()
{
    loadOffsetLib();
    loadInvertIndex();
    loadFileOfWordCount();
}

void WebPageQuery::loadOffsetLib()
{
    string _offsetLibPath; // 偏移库路径
    auto it = _confMap.find("offsetLibPath");
    if (it != _confMap.end())
    {
        _offsetLibPath = it->second;
    }
    ifstream ifs(_offsetLibPath);
    if (!ifs)
    {
        cerr << "ifs open " << _offsetLibPath << " error" << endl;
        exit(1);
    }

    string line;
    while (getline(ifs, line))
    {
        int docID;
        int startPos;
        int fileSize;
        ifs >> docID >> startPos >> fileSize;
        _offsetLib[docID] = make_pair(startPos, fileSize);
    }

    ifs.close();
}

void WebPageQuery::loadInvertIndex()
{
    string _invertIndexPath; // 倒排索引库路径
    auto it = _confMap.find("invertIndexPath");
    if (it != _confMap.end())
    {
        _invertIndexPath = it->second;
    }
    ifstream ifs(_invertIndexPath);
    if (!ifs)
    {
        cerr << "ifs open " << _invertIndexPath << " error" << endl;
        exit(1);
    }

    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        iss >> word;
        vector<pair<int, double>> indices;

        int index;
        double score;
        while (iss >> index >> score)
        {

            indices.push_back(make_pair(index, score));
        }

        _invertIndexTable[word] = indices;
    }

    ifs.close();
}

// void WebPageQuery::loadWordFrequency(){
//     string _wordFrequencyPath;// 全文词频
//     auto it = _confMap.find("wordFrequencyPath");
//     if(it != _confMap.end()){
//        _wordFrequencyPath  = it->second;
//     }
//     ifstream ifs(_wordFrequencyPath);
//     if(!ifs){
//         cerr << "ifs open " << _wordFrequencyPath << " error" << endl;
//         exit(1);
//     }
//
//     string line;
//     while(getline(ifs, line)){
//         string word;
//         int count;
//         ifs >> word >> count;
//         _wordFrequency[word] = count;
//     }
//
//     ifs.close();
// }

void WebPageQuery::loadFileOfWordCount()
{
    string _fileOfWordCountPath; // 包含某个词的文档数，即DF
    auto it = _confMap.find("fileOfWordCountPath");
    if (it != _confMap.end())
    {
        _fileOfWordCountPath = it->second;
    }
    ifstream ifs(_fileOfWordCountPath);
    if (!ifs)
    {
        cerr << "ifs open " << _fileOfWordCountPath << " error" << endl;
        exit(1);
    }

    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word;
        int cnt;
        while (iss >> word >> cnt)
        {
            _fileOfWordCount[word] = cnt;
        }
    }

    ifs.close();
}

// 计算词频存入一个map，根据TF-IDF算法计算每个关键字的权重，存入vector<double> _baseX。即为基准向量
void WebPageQuery::getQueryWordsWeithtVector(vector<string> &queryWord)
{
    // 查询词的词频统计
    for (auto elem : queryWord)
    {
        _queryWordFreq[elem]++;
    }

    // 计算基准向量
    clacWeight();
}

void WebPageQuery::clacWeight()
{
    vector<pair<string, double>> weights;

    for (auto e : _queryWordFreq)
    {
        string word = e.first;
        int TF = e.second; // 某个词在文章中出现的次数

        int DF = 0; // 出现这个词的文档数
        auto it = _fileOfWordCount.find(word);
        if (it == _fileOfWordCount.end())
        {
            DF = 1;
        }
        else
        {
            DF = it->second + 1;
        }

        int N = _offsetLib.size() + 1;           // 网页库文档总数
        double IDF = log2((double)N / (DF + 1)); // 逆文档频率
        double w = TF * IDF;
        weights.push_back(make_pair(word, w));
    }

    // 现在已经得到文档X的 <单词，未处理权重>
    // 将单词的权重进行归一化处理 w' = w /sqrt(w1^2 + w2^2 +...+ wn^2)
    double sum = 0;
    for (auto &e : weights)
    {
        sum += e.second * e.second;
    }
    sum = sqrt(sum);
    // 接下来计算word ---> w'
    for (auto &e : weights)
    {
        e.second /= sum; // w'
        // 将w' 存入容器作为基准向量
        _baseX.push_back(e.second);
    }

    cout << ">> 基准向量X = ";
    for (auto data : _baseX)
    {
        cout << data << " ";
    }
    cout << endl;
}

void WebPageQuery::keyIsExit()
{
    map<int, int> intersection;      // 网页ID，次数     统计网页ID出现的次数，用来求交集
    map<int, vector<double>> zhaobo; // 存放网页的余弦值

    for (auto keyWord : _queryWord)
    {
        auto it = _invertIndexTable.find(keyWord);
        if (it == _invertIndexTable.end())
        {
            // 只要其中有一个查询词不在索引表中，就认为没有找到相关的网页
            cout << ">> 索引表中未找到所有key" << endl;
            /* returnNoAnswer(); */
            return;
        }
        else
        {
            // 找到这个词，
            for (auto data : it->second)
            {                               // it->second 是一个vector<pair<int, double>>
                intersection[data.first]++; // data.first是网页ID
                zhaobo[data.first].push_back(data.second);
                /* cout << "ID = " << data.first << "   w = " << data.second << endl; */
            }
        }
    }

    // 如果所有关键词都能找到
    for (auto elem : intersection)
    {
        if ((size_t)elem.second == _queryWordFreq.size())
        {                                             // 网页ID出现的次数 = 关键词的个数时，即为交集
            vector<double> tmpY = zhaobo[elem.first]; // elem.first是相交的网页ID，zhaobo[first] 是一个vector容器，里面存储了相交网页的权重，即为向量Y
            // 计算余弦值
            double result = clacCos(_baseX, tmpY);
            /* cout << "余弦值 = " << result << endl; */
            // 将 余弦值 和 网页ID 存入multimap容器，按照余弦值逆序排列
            _retMap.insert(make_pair(result, elem.first));
        }
    }
}

// 计算余弦值
double WebPageQuery::clacCos(vector<double> X, vector<double> Y)
{
    double product = 0;
    for (size_t i = 0; i < X.size(); i++)
    {
        product += X[i] * Y[i];
    }

    double sum_a = 0;
    for (auto data : X)
    {
        sum_a += data * data;
    }
    double a = sqrt(sum_a);

    double sum_b = 0;
    for (auto data : Y)
    {
        sum_b += data * data;
    }
    double b = sqrt(sum_b);

    return product / (a * b);
}

void WebPageQuery::executeQuery()
{
    string pageLibPath;
    auto it = _confMap.find("pageLibPath");
    if (it != _confMap.end())
    {
        pageLibPath = it->second;
    }
    ifstream ifs(pageLibPath);
    if (!ifs)
    {
        cerr << "ifs open " << pageLibPath << " error" << endl;
        exit(1);
    }

    cout << ">> 匹配网页个数 = " << _retMap.size() << endl
         << endl;

    for (auto elem : _retMap)
    { // elem.second 就是要返回网页的网页ID
        // 去偏移库获取该网页ID对应的起始位置和网页大小
        // 然后读入，构建WebPage对象
        int startPos = _offsetLib[elem.second].first;
        int fileSize = _offsetLib[elem.second].second;

        char *buf = new char[fileSize + 1]();
        ifs.seekg(startPos);
        ifs.read(buf, fileSize);

        string doc(buf);
        WebPage page(doc, _conf, _jieba);
        // 将WebPage 对象存入容器
        _pageLib.push_back(make_pair(elem.second, page)); // elem.second 就是要返回网页的网页ID ,pair对的第二个元素是网页对象

        ifs.seekg(0); // 偏移到文档开头
        delete[] buf;
    }

    ifs.close();
}

string WebPageQuery::createJson()
{
    // 遍历unordered_map<int, WebPage> _pageLib;
    // 从每个网页对象中获取相关信息，构建json
    // ...

    // 创建一个 json 对象
    json obj;

    // 创建一个数组
    obj["WebPages"] = json::array();

    cout << ">> 仅返回最多8个优先级最高的网页" << endl;
    int cnt = 0;
    for (auto elem : _pageLib)
    {
        if (cnt < 8)
        {
            cout << ">> 匹配的网页ID = " << elem.second.getDocID() << endl;
            json webPageJson; // 每一个json对象用来存每篇网页信息，然后加入到数组

            string tmpTitle = elem.second.getTitle();
            Summary hightTitle;
            webPageJson["title"] = hightTitle.highlightKeywords(tmpTitle, _queryWord); // title 中如果有关键词，标红

            webPageJson["url"] = elem.second.getUrl();

            // 根据webContent生成动态摘要,并将关键字标红
            string tmpContent = elem.second.getContent();
            Summary hightSummary;
            webPageJson["summary"] = hightSummary.generateSummary(tmpContent, _queryWord);

            obj["WebPages"].push_back(webPageJson);
            cnt++;
        }
        else
        {
            break;
        }
    }

    // 将数组中的 json 对象转换为字符串
    string jsonString = obj.dump();

    // 再次打包
    json ret;
    ret["id"] = 200;
    ret["length"] = jsonString.length();
    ret["word"] = jsonString;

    return ret.dump();
}

string WebPageQuery::returnNoAnswer()
{
    /* string s1 = string("\033[1;31m"); */
    /* string s2 = string("\033[0m"); */
    /* string content = string("无相关网页"); */
    /* string msg = s1 + content + s2; */

    /* nlohmann::json json_obj; */
    /* json_obj["message"] = string(" "); */
    /* string json_str = json_obj.dump(); */

    string s = string(" ");
    json ret;
    ret["id"] = 200;
    ret["length"] = s.length();
    ret["word"] = s;

    return ret.dump();
}
