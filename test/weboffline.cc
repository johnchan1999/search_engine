#include <iostream>
#include "../Include/PageLib.h"
#include "../Include/PageLibProcessor.h"
using namespace std;


int main(){
    int beginTime, finishTime;
    beginTime = clock();
    // 配置文件路径
    string confPath = "../conf/WebConf.conf";
    Configuration *pConf = Configuration::getInstance(confPath);

    // 目录扫描类的对象
    DirScanner dirScanner;
    // 获取所有xml文件的绝对路径
    string xmlFilePath;
    map<string, string> conf = pConf->getConfigMap();

    auto it = conf.find("web_people");                                                                            
    if(it != conf.end()){
        xmlFilePath = it->second;
    }
    // 将所有的语料文件转为绝对路径
    dirScanner.traverse(xmlFilePath);
    
    FileProcessor process;

    // 开始生成临时库
    PageLib _page(*pConf, dirScanner, process);
   _page.createPageLib();
   _page.createOffsetLib();

   PageLibProcessor pp(*pConf);
   pp.doProcess();
    finishTime = clock();
    printf(">> 测试文件耗时 %d ms \n", finishTime-beginTime);
   std::cout << "---------------WebOffline excute pass------------" << std::endl;

}

