#include "../../Include/DirScanner.h"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

using std::cout;
using std::cerr;
using std::endl;


DirScanner::DirScanner()
{
    _filePath.reserve(128);
}


DirScanner::~DirScanner(){   }


void DirScanner::operator()(){  }


// 返回_vecFiles的引用
vector<string>& DirScanner::getFilePath()
{
    return _filePath;
}


// 获取某一目录下的所有文件
void DirScanner::traverse(const string &dirname)
{
    DIR* dir;
    struct dirent* entry;
    struct stat fileStat;

    if ((dir = opendir(dirname.c_str())) != nullptr) 
    {
        while ((entry = readdir(dir)) != nullptr) 
        {
            string fileName = entry->d_name;
            string path = dirname + "/" + fileName;

            if (stat(path.c_str(), &fileStat) == 0 && S_ISDIR(fileStat.st_mode)) 
            {
                if (fileName != "." && fileName != "..") {
                    traverse(path);
                }
            } 
            else 
            {
                if (fileName.length() > 4 && fileName.substr(fileName.length() - 4) == ".xml") {
                    _filePath.push_back(path);
                }
            }
        }
        closedir(dir);
    } else {
        cerr << "无法打开目录: " << dirname << endl;
    }
}


