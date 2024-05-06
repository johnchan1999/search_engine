#include "../../Include/WebPageSearch.h"

WebPageSearch::WebPageSearch(string keys)
    : _sought(keys)
{
}

WebPageSearch::~WebPageSearch() {}

string WebPageSearch::doQuery(WebPageQuery &query)
{
    // _conn 智能指针对象调用其内部的函数，将结果返回给Reactor
    // 执行查询请求
    std::cout << ">> 开始查询: " << _sought << std::endl;
    return query.do_query(_sought);
}
