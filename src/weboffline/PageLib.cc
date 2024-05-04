#include "../../Include/PageLib.h"
#include <iostream>
#include <fstream>
#include <utility>
#include <sys/stat.h>

using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::make_pair;

PageLib::PageLib(Configuration &conf, DirScanner &dirScanner, FileProcessor &fileProcess)
: _conf(conf)
, _dirScanner(dirScanner)
, _process(fileProcess)
{
    _pageLib.reserve(4096);
}

PageLib::~PageLib(){ }

// 获取每个语料文件的绝对路径
// 遍历容器，处理每一个xml文件
// 创建网页库
void PageLib::createPageLib()
{
    vector<string> _filePath = _dirScanner.getFilePath();           
    
    for(const string &_fileName : _filePath)
    {
        _process.parseRss(_fileName.c_str(), _pageLib);
    }

    storePageLib();
    cout << ">> 临时网页库已生成" << endl;
}

// 获取网页库的保存路径
// 获取临时网页库的保存路径
// 保存网页库
void PageLib::storePageLib()
{
    map<string, string> conf = _conf.getConfigMap();
    auto it = conf.find("tmpPagePath");
    string savePath;
    if(it != conf.end())
    {
        savePath = it->second;
    }

    ofstream ofs = openFile(savePath);                                                                                                      

    if(!ofs)
    {
        cerr << "ofs open " << savePath << " error " << endl;
        exit(1);
    }
 
    for(size_t idx = 0; idx < _pageLib.size(); ++idx)
    {
         ofs << "<doc>" << "\n"
             << "\t" << "<docid>" << idx + 1 << "</docid>" << "\n"
             << "\t" << "<url>" << _pageLib[idx].link << "</url>" << "\n"
             << "\t" << "<title>" << _pageLib[idx].title << "</title>" << "\n"
             << "\t" << "<content>" << _pageLib[idx].description << "</content>" << "\n"
             << "</doc>" << endl;
     }
     ofs.close();
}

// 获取网页库的位置
// 创建位置偏移库
void PageLib::createOffsetLib()
{
    map<string, string> conf = _conf.getConfigMap();
    auto it = conf.find("tmpPagePath"); 
    string fileName;
    if(it != conf.end())
    {
        fileName = it->second;
    }

    int id = 0;
    int startPos = 0;
    int fileSize = 0;
    ifstream ifs(fileName);
    if(!ifs){
        cerr << "ifs open " << fileName << " error " << endl;
        exit(1);
    }
    string line;
     while(getline(ifs, line)){
         if(line == "<doc>"){
             id++;
             startPos = (unsigned long)ifs.tellg()-6;
         }
         else if(line == "</doc>"){
             fileSize = (unsigned long)ifs.tellg() - startPos;
             _offsetLib[id] = make_pair(startPos, fileSize);
         }
     }
     ifs.close();

    storeOffsetLib();
    cout << ">> 临时位置偏移库已生成" << endl;
}


// 获取位置偏移库的存放路径
// 保存位置偏移库
void PageLib::storeOffsetLib()
{
    map<string, string> conf = _conf.getConfigMap();
    auto it = conf.find("tmpOffsetPath");       
    string savePath;
    if(it != conf.end())
    {
        savePath = it->second;
    }

    ofstream ofs(savePath);
    if(!ofs)
    {
        cerr << "ofs open " << savePath << " error " << endl;
        exit(1);
    }
    
    for(auto elem : _offsetLib)
    {
        ofs << elem.first << " " << elem.second.first << " " << elem.second.second << endl;
    }

    ofs.close();
}

// 检查目录是否存在                                                                                                                     
bool PageLib::isDirectoryExists(const string& directory)
{
    struct stat info;
     if (stat(directory.c_str(), &info) != 0) 
     {
         return false;
     }
     return (info.st_mode & S_IFDIR) != 0;
}

// 创建目录
bool PageLib::createDirectory(const string& directory)
{
    int status = mkdir(directory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    return status == 0;
}

// 获取目录路径                                                                                                                         
// 检查目录是否存在，若不存在则创建目录
// 创建目录并打开文件
ofstream PageLib::openFile(const string& filePath)
{
    string directory = filePath.substr(0, filePath.rfind('/'));
    if (!isDirectoryExists(directory)) 
    {
        if (!createDirectory(directory)) 
        {
            cerr << "Failed to create directory: " << directory << endl;
        }
     }
 
     ofstream ofs(filePath);
     if (!ofs) 
     {
         cerr << "Failed to open file: " << filePath << endl;
     }
     return ofs;

}


