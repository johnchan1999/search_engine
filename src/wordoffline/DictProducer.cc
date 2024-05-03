#include "../../Include/DictProducer.h"

#include "../../Include/CppJieba.h"
#include "../../Include/SplitToolEnglish.h"
#include <fstream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../../Include/Log4cpp.h"

namespace SearchEngine
{

    DictProducer::DictProducer(Configuration &conf)
        : _conf(conf), _splitTool(nullptr), _confpath(conf.getConfigMap()), _log()
    {
    }

    DictProducer::~DictProducer()
    {
        if (_splitTool)
        {
            delete _splitTool;
        }
    }

    int DictProducer::pathScann(const char *pathname)
    {
        _files.clear();
        struct stat storestat;
        int ret = stat(pathname, &storestat);
        if (-1 == ret)
        {
            printf("Error: open %s failed", pathname);
            _log.error(LOGPATTERN(__FILE__, __LINE__));
            exit(1);
        }

        if (S_ISREG(storestat.st_mode))
        {
            _files.push_back(pathname);
            return 0;
        }
        else if (S_ISDIR(storestat.st_mode))
        {
            DIR *pdir = opendir(pathname);
            if (nullptr == pdir)
            {
                printf("Error: open %s failed", pathname);
                _log.error(LOGPATTERN(__FILE__, __LINE__));
                exit(1);
            }

            struct dirent *pdirent = nullptr;
            while (NULL != (pdirent = readdir(pdir)))
            {
                if (DT_REG == pdirent->d_type)
                {
                    string fpath(pdirent->d_name);
                    fpath = string(pathname) + "/" + fpath;
                    _files.push_back(fpath);
                }
            }
            if (_files.size() == 0)
            {
                printf("Error: scan %s failed", pathname);
                _log.error(LOGPATTERN(__FILE__, __LINE__));
                exit(1);
            }
                }
        return 0;
    }

    void DictProducer::initEnDictAndIndex(const char *dictpath, const char *indexpath)
    {
        if (_splitTool)
        {
            delete _splitTool;
            _splitTool = new SplitToolEnglish();
        }
        else
        {
            _splitTool = new SplitToolEnglish();
        }
        _splitTool->initStopWords(_conf.getEnStopWordList());
        buildEnDict();
        buildEnIndex();
        storeDict(dictpath);
        storeIndex(indexpath);
        _dict.clear();
        _indexTable.clear();
    }

    void DictProducer::initCnDictAndIndex(const char *dictpath, const char *indexpath)
    {
        if (_splitTool)
        {
            delete _splitTool;
            _splitTool = new CppJieba();
        }
        else
        {
            _splitTool = new CppJieba();
        }
        _splitTool->initStopWords(_conf.getCnStopWordList());
        buildCnDict();
        buildCnIndex();
        storeDict(dictpath);
        storeIndex(indexpath);
        _dict.clear();
        _indexTable.clear();
    }

    // 创建英文字典
    void DictProducer::buildEnDict()
    {
        pathScann(_confpath["englishword"].c_str());

        for (auto &filepath : _files)
        {
            std::ifstream input(filepath, std::ios_base::ate);
            if (!input.is_open())
            {
                printf("Error:open %s failed", filepath.c_str());
                _log.error(LOGPATTERN(__FILE__, __LINE__));
                exit(1);
            }

            int length = input.tellg();
            input.seekg(std::ios_base::beg);
            if (length > 65536)
            {
                readBigFile(input, length);
            }
            else
            {
                readSmallFile(input, length);
            }
            input.close();
        }

        for (auto &i : _temdict)
        {
            _dict.push_back(i);
        }
        if (0 == _dict.size())
        {
            printf("Error: scan %s failed", _confpath["englishword"].c_str());
            _log.error(LOGPATTERN(__FILE__, __LINE__));
            exit(1);
        }
        _temdict.clear();
    }

    // 创建中文字典
    void DictProducer::buildCnDict()
    {
        // 读取配置文件中为中文词的路径
        pathScann(_confpath["chineseword"].c_str());

        for (auto &filepath : _files)
        {
            std::cout << filepath << "\n";
            _log.info(LOGPATTERN(__FILE__, __LINE__));
            std::ifstream input(filepath, std::ios_base::ate);

            if (!input.is_open())
            {
                _log.error(LOGPATTERN(__FILE__, __LINE__));
                exit(1);
            }

            int length = input.tellg();
            input.seekg(std::ios_base::beg);

            if (length > 65536)
            {
                readBigFile(input, length);
            }
            else
            {
                readSmallFile(input, length);
            }
            input.close();
        }

        for (auto &i : _temdict)
        {
            _dict.push_back(i);
        }
        _temdict.clear();
    }

    // 建立中文索引
    void DictProducer::buildCnIndex()
    {
        _indexTable.clear();
        int line = 1;
        for (auto &i : _dict)
        {
            string target = i.first;
            string comstr = target.substr(0, 3);
            auto pmap = _indexTable.find(comstr);

            if (pmap != _indexTable.end())
            {
                _indexTable[comstr].insert(line);
            }
            else
            {
                _indexTable.insert({comstr, {line}});
            }
            ++line;
        }
    }

    // 建立英文索引
    void DictProducer::buildEnIndex()
    {
        _indexTable.clear();
        int line = 1;
        for (auto &i : _dict)
        {
            string target = i.first;
            char firstAlpha = i.first[0];
            if ((firstAlpha >= 97 && firstAlpha <= 122))
            {
                string comstr(1, firstAlpha);
                auto pmap = _indexTable.find(comstr);
                if (pmap != _indexTable.end())
                {
                    _indexTable[comstr].insert(line);
                }
                else
                {
                    _indexTable[comstr] = {line};
                }
            }
            else
            {
                continue;
            }
            ++line;
        }
    }

    // 读小文件
    void DictProducer::readSmallFile(std::ifstream &input, int length)
    {
        char *buf = new char[length + 1];
        input.read(buf, length + 1);
       // std::cout << buf << "\n";

        vector<string> vec(_splitTool->cut(buf));

        for (auto &word : vec)
        {
            if (word.size() > 0)
            {
                if (_temdict.count(word))
                {
                    ++_temdict[word];
                }
                else
                {
                    _temdict[word] = 1;
                }
            }
        }
        free(buf);
    }

    // 读大文件
    void DictProducer::readBigFile(std::ifstream &input, int length)
    {
        int ret = length;
        int onesize = 65536;
        while (ret > 0)
        {
            if (ret > onesize)
                readSmallFile(input, onesize);
            else
                readSmallFile(input, ret);

            ret -= onesize;
        }
    }

    // 将词典写入文件
    void DictProducer::storeDict(const char *filepath)
    {
        std::ofstream output(filepath, std::ios_base::out);
        for (auto &p : _dict)
        {
            string tem = p.first + " " + std::to_string(p.second) + "\n";
            output.write(tem.c_str(), tem.size());
        }
    }

    // 将索引文件写入文件
    void DictProducer::storeIndex(const char *filepath)
    {
        std::ofstream output(filepath, std::ios_base::out);
        if (!output.is_open())
        {
            printf("Error: create %s failed", filepath);
            _log.error(LOGPATTERN(__FILE__, __LINE__));
            exit(1);
        }
        for (auto &p : _indexTable)
        {
            string tem = p.first + " ";
            for (auto &k : p.second)
            {
                tem += " " + std::to_string(k) + " ";
            }
            tem += "\n";
            output.write(tem.c_str(), tem.size());
        }
    }

    // 查看文件路径，作为测试用
    void DictProducer::showFiles() const
    {
        for (auto &i : _files)
        {
            std::cout << i << std::endl;
        }
    }

    // 查看词典，作为测试用
    void DictProducer::showDict() const
    {
        for (auto &i : _dict)
        {
            std::cout << i.first << " + " << i.second << std::endl;
        }
    }

    // 获取文件的绝对路径
    void DictProducer::getFiles()
    {
        for (auto &i : _confpath)
        {
            std::cout << i.first << " + " << i.second << std::endl;
        }
    }
};