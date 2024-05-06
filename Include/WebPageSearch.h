#pragma once
#ifndef __ALOPEX_WebPageSearch_HPP__
#define __ALOPEX_WebPageSearch_HPP__


#include "WebPageQuery.h"
#include <string>
#include <memory>
using std::string;
using std::shared_ptr;

class WebPageSearch
{
public:
    WebPageSearch(string keys);
    /* WebPageSearch(string keys); */
    ~WebPageSearch();
    string doQuery(WebPageQuery &query);

private:
    string _sought;
};


#endif
