#ifndef __SPLITTOOLENLISH_H__
#define __SPLITTOOLENLISH_H__

#include "SplitTool.h"
#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>

namespace SearchEngine
{
    using std::set;
    using std::string;
    using std::vector;

    class SplitToolEnglish
        : public SplitTool
    {
    public:
        SplitToolEnglish() {}
        ~SplitToolEnglish() {}

        vector<string> cut(const string &sentence)
        {
            vector<string> result;
            result.reserve(1024);
            std::istringstream istr(sentence, std::ios_base::in);
            if (!istr.good())
            {
                std::cerr << "istringstream not good" << std::endl;
            }
            string word;

            while (istr >> word)
            {
                string temword;
                for (int i = 0; i < word.size(); ++i)
                {
                    char ch = word[i];
                    if (std::isalpha(ch))
                    {
                        if (std::isupper(ch))
                        {
                            char c = std::tolower(ch);
                            temword.push_back(c);
                        }
                        else
                        {
                            temword.push_back(ch);
                        }
                    }
                    else
                    {
                        break;
                    }
                }

                if (0 == _stopWords.count(temword) && temword.size() != 1)
                {
                    result.push_back(temword);
                }
            }
            return result;
        }

        void initStopWords(const set<string> &lhs)
        {
            std::cout << ">> 初始化英文停用词数量：" << lhs.size() << std::endl;
            _stopWords = lhs;
        }

    private:
        set<string> _stopWords;
    };

}; 

#endif