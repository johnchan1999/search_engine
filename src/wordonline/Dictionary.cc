#include "../../Include/Dictionary.h"
#include <sys/types.h>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>

namespace SearchEngine
{
    Dictionary *Dictionary::_pdict = nullptr;

    Dictionary::Dictionary()
        : _log() {}

    Dictionary::~Dictionary()
    {
    }

    Dictionary *Dictionary::createInstance()
    {
        if (!_pdict)
        {
            _pdict = new Dictionary();
        }
        return _pdict;
    }

    void Dictionary::pathscan(const string &pathname)
    {
        _log.info(LOGPATTERN(__FILE__, __LINE__));
        struct stat storestat;
        int ret = stat(pathname.c_str(), &storestat);
        if (-1 == ret)
        {
            _log.error(LOGPATTERN(__FILE__, __LINE__) + " pathname wrong");
            exit(1);
        }

        if (S_ISREG(storestat.st_mode))
        {
            _files.push_back(string(pathname));
            return;
        }
        else if (S_ISDIR(storestat.st_mode))
        {
            DIR *pdir = opendir(pathname.c_str());
            if (nullptr == pdir)
            {
                _log.error(LOGPATTERN(__FILE__, __LINE__) + " opendir wrong");
                exit(1);
            }

            struct dirent *pdirent = nullptr;
            while (NULL != (pdirent = readdir(pdir)))
            {
                if (DT_REG == pdirent->d_type)
                {
                    string fpath(pdirent->d_name);
                    if (fpath == "_CnDict.dat")
                    {
                        fpath += "1";
                    }
                    else if (fpath == "_EnDict.dat")
                    {
                        fpath += "2";
                    }
                    else if (fpath == "_CnIndexTable.dat")
                    {
                        fpath += "3";
                    }
                    else if (fpath == "_EnIndexTable.dat")
                    {
                        fpath += "4";
                    }
                    // 可打开的路径
                    fpath = pathname + "/" + fpath;
                    // std::cout << fpath << std::endl;
                    _files.push_back(fpath);
                }
            }
        }
    }

    void Dictionary::init(const string &dictpath) // 通过词典文件路径初始化词典
    {
        // 扫描词典目录内的文件
        pathscan(dictpath);
        // 从./dat中读取到有效的信息
        readfromdat();
        _log.info(LOGPATTERN(__FILE__, __LINE__) + " init finish");
    }

    void Dictionary::readEnDict(const string &filepath)
    {
        std::ifstream input;
        input.open(filepath, std::ios_base::in);
        if (input.is_open() == false)
        {
            _log.error(LOGPATTERN(__FILE__, __LINE__) + " readfromdat");
            exit(1);
        }
        std::string line;
        while (std::getline(input, line))
        {
            std::istringstream istr(line);
            string word;
            int frequency;
            if (istr >> word >> frequency)
            {
                _endict.push_back({word, frequency});
            }
        }
    }

    void Dictionary::readCnDict(const string &filepath)
    {
        std::ifstream input;
        input.open(filepath, std::ios_base::in);
        if (input.is_open() == false)
        {
            _log.error(LOGPATTERN(__FILE__, __LINE__) + " readfromdat");
            exit(1);
        }
        std::string line;
        while (std::getline(input, line))
        {
            std::istringstream istr(line);
            string word;
            int frequency;
            if (istr >> word >> frequency)
            {
                _cndict.push_back({word, frequency});
            }
        }
    }

    void Dictionary::readEnIndex(const string &filepath)
    {
        std::ifstream input;
        input.open(filepath, std::ios_base::in);
        if (input.is_open() == false)
        {
            _log.error(LOGPATTERN(__FILE__, __LINE__) + " readfromdat");
            exit(1);
        }
        string line;
        while (std::getline(input, line))
        {
            std::istringstream istr(line);
            string word;
            istr >> word;

            int indexline = 0;
            while (istr >> indexline)
            {
                _enIndexTable[word].insert(indexline);
            }
        }
    }

    void Dictionary::readCnIndex(const string &filepath)
    {
        std::ifstream input;
        input.open(filepath, std::ios_base::in);
        if (input.is_open() == false)
        {
            _log.error(LOGPATTERN(__FILE__, __LINE__) + " readfromdat");
            exit(1);
        }
        string line;
        while (std::getline(input, line))
        {
            std::istringstream istr(line);
            string word;
            istr >> word;

            int indexline = 0;
            while (istr >> indexline)
            {
                _cnIndexTable[word].insert(indexline);
            }
        }
    }

    void Dictionary::readfromdat()
    {
        for (auto &i : _files)
        {
            char ch = i.back();

            if (ch == '2')
            {
                i.pop_back();
                readEnDict(i);
            }
            else if (ch == '1')
            {
                i.pop_back();
                readCnDict(i);
            }
            else if (ch == '4')
            {
                i.pop_back();
                readEnIndex(i);
            }
            else if (ch == '3')
            {
                i.pop_back();
                readCnIndex(i);
            }
        }
    }

    vector<pair<string, int>> &Dictionary::getenDict() // 获取词典
    {
        _log.info(LOGPATTERN(__FILE__, __LINE__) + " getenDict");
        return _endict;
    }

    vector<pair<string, int>> &Dictionary::getcnDict() // 获取词典
    {
        _log.info(LOGPATTERN(__FILE__, __LINE__) + " getcnDict");
        return _cndict;
    }

    map<string, set<int>> &Dictionary::getEnIndexTable() // 获取索引表
    {
        _log.info(LOGPATTERN(__FILE__, __LINE__) + " getEnIndexTable");
        return _enIndexTable;
    }

    map<string, set<int>> &Dictionary::getCnIndexTable() // 获取索引表
    {
        _log.info(LOGPATTERN(__FILE__, __LINE__) + " getCnIndexTable");
        return _cnIndexTable;
    }

    void Dictionary::destroy()
    {
        if (_pdict)
        {
            delete _pdict;
            _pdict = nullptr;
        }
    }

}; // end of SearchEngine