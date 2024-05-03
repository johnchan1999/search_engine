#include "../Include/KeyRecommander.h"
#include "../Include/DictProducer.h"
#include "../Include/Configuration.h"

using OurPool::TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using SearchEngine::Dictionary;
using SearchEngine::DictProducer;
using SearchEngine::KeyRecommander;
using std::string;

int main()
{
    int beginTime, finishTime;
    beginTime = clock();
    TcpConnectionPtr tp = nullptr;
    Dictionary *diction = Dictionary::createInstance();
    diction->init("../data/datfile");
    string str = "中国";
    std::cout << ">> Your word：" << str << std::endl;
    KeyRecommander rec(str, tp, *diction);
    rec.execute();
    rec.response();
    finishTime = clock();
    printf(">> test time spend %d ms \n", finishTime - beginTime);
    std::cout << "---------WordOnlineTest excute pass----------" << std::endl;
}