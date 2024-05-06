#pragma once
#ifndef __Summary_H__
#define __Summary_H__

#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <iterator>

using std::string;
using std::vector;
using std::wstring;
using std::wstring_convert;

class Summary
{
public:
    Summary() {}
    ~Summary() {}
    string highlightKeywords(const string &sentence, const vector<string> &keywords);
    string generateSummary(const string &content, const vector<string> &keywords);

private:
    wstring utf8ToWstring(const string &str);
    string wstringToUtf8(const wstring &wstr);

private:
};

#endif
