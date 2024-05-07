#ifndef __LRUCache_H__
#define __LRUCache_H__

#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <iterator>
#include <utility>
#include <fstream>
#include <sstream>
using namespace std;
class LRUCache
{
public:
    LRUCache(int);
    LRUCache(const LRUCache &);
    string getelement(const string &);
    bool addelement(const string &, const string &);
    void readfromfile(const string &filename);
    void writetofile(const string &filename);
    void update(LRUCache &);
    list<pair<string, string>> &getpendingupdatelist();

private:
    unordered_map<string, list<pair<string, string>>::iterator> _hashmap;
    list<pair<string, string>> _resultlist;
    list<pair<string, string>> _pendingupdatelist;
    int _capacity;
    int _size;
};
#endif