#include <iostream>
#include <stdio.h>
#include "../Include/WebPageSearch.h"
using namespace std;


int main(){
    int beginTime, finishTime;
    beginTime = clock();
    /* TcpConnectionPtr conn = make_shared<TcpConnection>(); */ 
    string confPath = "../conf/WebConf.conf";
    Configuration *pConf = Configuration::getInstance(confPath);
    
    /* WebPageSearch search(keys, conn); */
    string keys = "供电";
    std::cout << ">> 输入词：" << keys << std::endl;
    WebPageQuery query(*pConf);
    WebPageSearch search(keys);
    search.doQuery(query);
    keys.erase(keys.begin(), keys.end());
    finishTime = clock();
    printf(">> 测试文件耗时 %d ms \n", finishTime-beginTime);
    std::cout << "---------------WebOnlineTest excute pass-------------" << std::endl;
}

