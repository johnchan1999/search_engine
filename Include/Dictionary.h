#ifndef __DICTIOINARY_H__
#define __DICTIOINARY_H__

#include "Log4cpp.h"
#include <vector>
#include <string>
#include <map>
#include <set>

namespace SearchEngine
{

    using std::map;
    using std::pair;
    using std::set;
    using std::string;
    using std::vector;

    class Dictionary
    {
    public:
        vector<pair<string, int>> &getenDict();   // 获取词典
        vector<pair<string, int>> &getcnDict();   // 获取词典
        void readEnDict(const string &);          // 读取英文词典
        void readCnDict(const string &);          // 读取中文词典
        void readEnIndex(const string &);         // 读取英文索引
        void readCnIndex(const string &);         // 读取中文词典
        map<string, set<int>> &getEnIndexTable(); // 获取英文索引表
        map<string, set<int>> &getCnIndexTable(); // 获取中文索引表
        static Dictionary *createInstance();      // 静态函数
        void init(const string &dictpath);        // 通过词典文件路径初始化词典
        void pathscan(const string &);            // 扫描存放词典和索引文件的目录
        void readfromdat();                       // 读取.dat文件
        static void destroy();                    // 手动销毁Dictionary类对象

    private:
        vector<pair<string, int>> _endict;   // 存放词典文件数据：单词 + 词频
        vector<pair<string, int>> _cndict;   // 存放词典文件数据：单词 + 词频
        map<string, set<int>> _enIndexTable; // 存放索引文件数据：字母 + 词典文件行号的集合
        map<string, set<int>> _cnIndexTable; // 存放索引文件数据：字母 + 词典文件行号的集合
        static Dictionary *_pdict;           // 用来指向Dictionary类单例对象的指针
        vector<string> _files;               // 0，1：代表中文词典和英文词典，2，3：代表中文词典和英文索引
        MyLog _log;                          // 日志类对象

    private:
        Dictionary();
        ~Dictionary();
    };
}; // end of SearchEngine

#endif