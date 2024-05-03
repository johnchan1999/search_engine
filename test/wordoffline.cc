#include "../Include/DictProducer.h"
#include <sys/unistd.h>
using namespace SearchEngine;
int main()
{
    int beginTime, finishTime;
    beginTime = clock();
    Configuration conf("../conf/DictConf.conf");
    DictProducer dic(conf);
    dic.initCnDictAndIndex("../data/datfile/_CnDict.dat", "../data/datfile/_CnIndexTable.dat");
    std::cout << ">> Read the Chinese Dictionary and Generate an Index File"
              << "../data/_CnDict.dat  "
              << "../data/_CnIndexTable.dat" << std::endl;

    dic.initEnDictAndIndex("../data/datfile/_EnDict.dat", "../data/datfile/_EnIndexTable.dat");
    std::cout << ">> Read the English Dictionary and Generate an Index File"
              << "../data/_EnDict.dat  "
              << "../data/_EnIndexTable.dat" << std::endl;
    finishTime = clock();
    printf(">> test time spend %d ms \n", finishTime - beginTime);
    std::cout << "------------WordOfflineTest excute pass---------" << std::endl;
}