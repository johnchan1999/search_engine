#include "../../Include/Summary.h"
#include <iostream>

wstring Summary::utf8ToWstring(const string &str)
{
    wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
}

string Summary::wstringToUtf8(const wstring &wstr)
{
    wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(wstr);
}

// 标红关键字
string Summary::highlightKeywords(const string &sentence, const vector<string> &keywords)
{
    string highlightedSentence = sentence;
    string startTag = string("\033[1;31m");
    string endTag = string("\033[0m");

    for (const auto &keyword : keywords)
    {
        size_t pos = highlightedSentence.find(keyword);
        while (pos != std::string::npos)
        {
            highlightedSentence.replace(pos, keyword.length(), startTag + keyword + endTag);
            pos = highlightedSentence.find(keyword, pos + startTag.length() + endTag.length());
        }
    }

    return highlightedSentence;
}

string Summary::generateSummary(const string &content, const vector<string> &keywords)
{
    vector<wstring> sentencesVec; // 存放按照标点切割后的句子

    // 将content字符串按照标点符号分割成句子
    wstring wideContent = utf8ToWstring(content);
    wstring separators = L",.!?;，。！？；"; // 分隔符
    size_t start = 0;
    while (start < wideContent.length())
    {
        size_t end = wideContent.find_first_of(separators, start);
        if (end == std::wstring::npos)
        {
            end = wideContent.length();
        }
        sentencesVec.push_back(wideContent.substr(start, end + 1 - start));
        start = end + 1; // 偏移到下一个句子的开头
    }

    vector<string> summary; // 存放包含关键字的句子

    // 根据keywords查找包含关键词的句子
    for (auto &sentence : sentencesVec)
    {
        wstring lowercaseSentence = sentence;
        transform(lowercaseSentence.begin(), lowercaseSentence.end(), lowercaseSentence.begin(), ::tolower);
        for (const auto &keyword : keywords)
        {
            wstring lowercaseKeyword = utf8ToWstring(keyword);
            transform(lowercaseKeyword.begin(), lowercaseKeyword.end(), lowercaseKeyword.begin(), ::tolower);
            if (lowercaseSentence.find(lowercaseKeyword) != std::wstring::npos)
            {
                /* summary.push_back(sentence); */
                string lightStr = highlightKeywords(wstringToUtf8(sentence), keywords);
                summary.push_back(lightStr);
                break;
            }
        }
    }
    string ret;
    for (const auto &sentence : summary)
    {
        /* ret.append(wstringToUtf8(sentence)); */
        ret.append(sentence);
    }

    return ret;
}

// 测试代码
// int main() {
//    std::string content = "这是一段示例的内容。其中包含了一些句子。This is an example sentence. Another example sentence. 这是需要摘要的内容。";
//    /* std::vector<std::string> keywords = { "示例", "摘要" }; */
//    std::vector<std::string> keywords = { "is", "摘要", "赵博" };
//    /* std::vector<std::string> keywords = { "is", "sentence" }; */
//
//    Summary mysummary;
//    std::string summary = mysummary.generateSummary(content, keywords);
//    std::cout << summary << std::endl;
//    return 0;
//}