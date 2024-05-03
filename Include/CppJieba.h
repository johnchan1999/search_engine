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
            char userdict[] = "../data/dict/user.dict.utf8";
            char idfpath[] = "../data/dict/idf.utf8";
            char stopwords[] = "../data/dict/stop_words.utf8";

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
                    //std::cout << w << "\n";
                }else{
                    //std::cout << w << "\n";
                }
            }
            // std::cout << ">>" << result << "\n";
            // std::cout << ">>  CppJieba Cut the Number of Chinese Words：" << result.size() << "\n";
            return result;
        }

        void initStopWords(const set<string> &lhs)
        {
            std::cout << ">>  Initialize the Number of Chinese Stop Words：" << lhs.size() << std::endl;
            _stopWords = lhs;
        }

    private:
        set<string> _stopWords;
    };

};

#endif