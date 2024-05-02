#ifndef __SPLITTOOL_H__
#define __SPLITTOOL_H__

#include <vector>
#include <string>
#include <set>

namespace SearchEngine 
{
using std::vector;
using std::string;
using std::set;

class SplitTool
{
public:
    SplitTool(){ }  
    virtual ~SplitTool(){ }
    virtual vector<string> cut(const string & sentence) = 0; 
    virtual void initStopWords(const set<string> &) = 0;
};

}; 
#endif