#ifndef __DICTPRODUCER_H__
#define __DICTPRODUCER_H__

#include "Configuration.h"
#include "Log4cpp.h"
#include <vector>
#include <string>
#include <set>
#include <map>
#include <unordered_map>

namespace SearchEngine
{
    using std::map;
    using std::pair;
    using std::set;
    using std::string;
    using std::unordered_map;
    using std::vector;

    class SplitTool;

    class DictProducer
    {
    public:
        DictProducer(Configuration &);
        ~DictProducer();
        void initEnDictAndIndex(const char *, const char *);
        void initCnDictAndIndex(const char *, const char *);
        void showFiles() const; // 查看文件路径（测试用）
        void showDict() const;  // 查看词典（测试用）
        void getFiles();        // 获取文件的绝对路径（测试用）

    private:
        void readSmallFile(std::ifstream &, int); // 读小文件
        void readBigFile(std::ifstream &, int);   // 读大文件
        int pathScann(const char *);              // 路径扫描
        void storeIndex(const char *);            // 存储索引表到指定文件
        void storeDict(const char *);             // 存储词典到指定文件
        void buildEnDict();                       // 读取英文词典文件
        void buildCnDict();                       // 创建中文字典文件
        void buildCnIndex();                      // 建立中文索引
        void buildEnIndex();                      // 建立英文索引

    private:
        vector<pair<string, int>> _dict;             // 词典：短语 + 词频
        unordered_map<string, set<int>> _indexTable; // 存放索引: 首个词 + 词典文件的行数集合
        map<string, string> _confpath;               // 配置文件内容: 关键词 + 路径
        Configuration &_conf;                        // 配置文件类
        vector<string> _files;                       // 语料库文件的绝对路径集合
        unordered_map<string, int> _temdict;         // 临时词典：用来过滤重复关键词
        SplitTool *_splitTool;                       // 分词工具:SplitToolCppJieba(中文),SplitiToolEnglish（英文）
        MyLog _log;                                  // 日志对象
    };

};

#endif