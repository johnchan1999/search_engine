#ifndef __CPPJIEBA_H__
#define __CPPJIEBA_H__

#include "SplitTool.h"
#include "cppjieba/Jieba.hpp"
#include "isChinese.h"
#include <sstream>
#include <set>

using std::set;

namespace SearchEngine
{
    class CppJieba
        : public SplitTool
    {
    public:
        CppJieba() {}
        ~CppJieba() {}

        vector<string> cut(const string &sentence)
        {
            char dictpath[] = "../data/dict/jieba.dict.utf8";
            char hmmmodetl[] = "../data/dict/hmm_model.utf8";
            char userdict[] = "../datadict/user.dict.utf8";
            char idfpath[] = "../datadict/idf.utf8";
            char stopwords[] = "../datadict/stop_words.utf8";

            cppjieba::Jieba jieba(dictpath, hmmmodetl, userdict, idfpath, stopwords);

            vector<string> words;
            vector<string> result;
            string tem;
            string toCut;
            std::istringstream istr(sentence);

            while (istr >> tem)
            {
                toCut += tem;
            }
            jieba.Cut(toCut, words, true);
            result.reserve(words.size());

            for (int i = 0; i < words.size(); ++i)
            {
                std::string w(words[i]);
                char ch = w[0];
                if (0 == _stopWords.count(w) && getByteNum_utf8(ch) == 3)
                {
                    result.push_back(w);
                }
            }

            return result;
        }

        void initStopWords(const set<string> &lhs)
        {
            std::cout << ">> 初始化中文停用词数量：" << lhs.size() << std::endl;
            _stopWords = lhs;
        }

    private:
        set<string> _stopWords;
    };

};

#endif