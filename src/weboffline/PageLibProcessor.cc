#include "../../Include/PageLibProcessor.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

using std::cerr;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::make_pair;


PageLibProcessor::PageLibProcessor(Configuration &conf)
: _conf(conf)
, _jieba() {    }

PageLibProcessor::~PageLibProcessor(){  }

void PageLibProcessor::doProcess()
{
    readInfoFromFile();
    cutRedundantPages();
    storeOnDisk();    
}

//获取临时偏移库的路径
//获取临时网页库的路径
//根据配置信息读取网页库和位置偏移库的内容 
void PageLibProcessor::readInfoFromFile()
{
    map<string, string> confMap = _conf.getConfigMap();
    auto it1 = confMap.find("tmpPagePath");
    if(it1 != confMap.end())
    {
        _tmpPagePath = it1->second;
    }
    
    auto it2 = confMap.find("tmpOffsetPath");
    if(it2 != confMap.end())
    {
        _tmpOffsetPath = it2->second;
    }
    
    auto it3 = confMap.find("pageLibPath");
    if(it3 != confMap.end())
    {
        _pageLibPath = it3->second;
    }
    
    auto it4 = confMap.find("offsetLibPath");
    if(it4 != confMap.end()){
        _offsetLibPath = it4->second;
    }

    auto it5 = confMap.find("invertIndexPath");         // 倒排索引库的路径
    if(it5 != confMap.end())
    {
        _invertIndexPath = it5->second;
    }

    // 出现某个单词的文档数
    auto it6 = confMap.find("fileOfWordCountPath");
    if(it6 != confMap.end())
    {
        _fileOfWordCountPath = it6->second;
    }
}


//对冗余的网页进行去重 
void PageLibProcessor::cutRedundantPages()
{
    ifstream pageFile(_tmpPagePath);           
    if(!pageFile){
        cerr << "pageFile open " << _tmpPagePath << " error " << endl;
        exit(1);
    }
    ifstream offsetFile(_tmpOffsetPath);      
    if (!offsetFile){
        cerr << "offsetFile open " << _tmpOffsetPath << " error " << endl;
        exit(-1);
    }
    string line;
    int docID;
    off_t offset, fileSize;
    while (getline(offsetFile, line))
    { 
        stringstream ss(line);
        ss >> docID >> offset >> fileSize;
        char *buf = new char[fileSize + 1]();
        pageFile.read(buf, fileSize);
        string doc(buf);
        WebPage page(doc, _conf, _jieba);
        delete[] buf;
        
        auto it =std::find(_pageLib.begin(), _pageLib.end(), page);
        if(it != _pageLib.end())
        {
            cout << "重复" << "\n";
        }
        else
        {
            cout << "插入" << "\n";
            _pageLib.push_back(std::move(page));
        }



    
    }

    offsetFile.close();
    pageFile.close();

    cout << ">> 去重完毕" << endl;
}


//创建倒排索引表 
void PageLibProcessor::buildInvertIndexTable(){
    // 二维数组每一行对应一篇网页，这一行的每一列存储该篇网页每个词语以及它的w
    vector<vector<pair<string, double>>> weights(_pageLib.size()); 

    // 遍历存放网页对象的动态数组
    for (size_t i = 0; i < _pageLib.size(); i++){
                                                                                    
        int docID = _pageLib[i].getDocID();                                         // 每篇网页的词频map
        unordered_map<string, int> &wordsMap = _pageLib[i].getWordsMap();
        for (auto &e : wordsMap)
        {   
            string word = e.first;                                                   // 单词
            int TF = e.second;                                                       // 某个词在这篇文章中出现的次数

            int DF = 0;                                                              // 出现这个词的文档数
            auto it = _fileOfWordCountMap.find(word);                                // 先到容器查找
            if(it != _fileOfWordCountMap.end())
            {
                DF = it->second;                                                     // 找到直接用
            }
            else{
                for(size_t i=0; i<_pageLib.size(); i++){
                    if(_pageLib[i].getWordsMap().find(word) != _pageLib[i].getWordsMap().end())
                    {
                        DF++;
                    }
                }
                _fileOfWordCountMap[word] = DF;                                     // 将DF即出现这个单词的文档数存入容器
            }
            int N = _pageLib.size();                                                // 网页库文档总数
            double IDF = log2((double)N / (DF + 1));                                // 逆文档频率
            double w = TF * IDF;
            weights[i].push_back(make_pair(word, w));
        }

        //将单词的权重进行归一化处理 W = w /sqrt(w1^2 + w2^2 +...+ wn^2)
        vector<pair<string, double>> &wordsVec = weights[i];                        //i号文档的 <单词，未处理权重>
        double sum = 0;
        for (auto &e : wordsVec){
            sum += e.second * e.second;
        }
        sum = sqrt(sum);
        for (auto &elem : wordsVec){
            elem.second /= sum;                                                     // 每一个词的w'     (word, w')
            _invertIndexTable[elem.first].push_back(make_pair(docID, elem.second)); //插入结果到倒排索引表
        }
    }
    storeFileOfWordCount();
}

// 将_fileOfWordCountMap的内容 写入文件   在线部分使用
void PageLibProcessor::storeFileOfWordCount()
{
    ofstream ofs(_fileOfWordCountPath, ofstream::out);
    if(!ofs){
        cerr << "ofs open " << _fileOfWordCountPath << " error" << endl;
        exit(1);
    }
    for(auto elem : _fileOfWordCountMap){
        ofs << elem.first << " " << elem.second << endl;
    }
    // 写完后关闭文件
    ofs.close();
}

//将经过预处理之后的网页库、位置偏移库和倒排索引表写回到磁盘上 
void PageLibProcessor::storeOnDisk()
{
    ofstream pageFile(_pageLibPath, ofstream::out);
    ofstream offsetFile(_offsetLibPath, ofstream::out);
    ofstream invertIndexFile(_invertIndexPath, ofstream::out);
    if (!pageFile.good() || !offsetFile.good() || !invertIndexFile.good())
    {
        cerr << "open file error" << endl;
        exit(-1);
    }

    //写入新的 网页库 与 偏移库
    for (size_t i = 0; i < _pageLib.size(); i++)
    {
        string doc = _pageLib[i].getDoc();
        off_t start = pageFile.tellp();
        pageFile << doc;
        off_t end = pageFile.tellp();
        offsetFile << _pageLib[i].getDocID() << " " << start << " " << end-start << endl; // 网页ID，起始位置，网页大小
    }
    cout << ">> 正式网页库，偏移库创建成功" << endl;

    buildInvertIndexTable();

    for (auto &e : _invertIndexTable)
    {
        invertIndexFile << e.first << " ";
        for (auto &data : e.second){                                                    // e.second 是一个动态数组 vector<pair<int, double>>
            invertIndexFile << data.first << " " << data.second << " ";
        }
        invertIndexFile << endl;
    }

    pageFile.close();
    offsetFile.close();
    invertIndexFile.close();
    cout << ">> 倒排索引库创建成功" << endl;
}

