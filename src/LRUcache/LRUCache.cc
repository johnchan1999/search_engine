#include "LRUCache.h"
LRUCache::LRUCache(int capacity) : _capacity(capacity), _size(0) {}
LRUCache::LRUCache(const LRUCache &cache) : _hashmap(cache._hashmap), _resultlist(cache._resultlist), _pendingupdatelist(cache._pendingupdatelist), _capacity(cache._capacity) {}
string LRUCache::getelement(const string &key)
{
    auto it = _hashmap.find(key);
    if (it != _hashmap.end())
    {
        _resultlist.splice(_resultlist.begin(), _resultlist, it->second);
        _pendingupdatelist.push_back(make_pair(key, _resultlist.begin()->second));
        return _resultlist.begin()->second;
    }
    else
    {
        return "";
    }
}
void LRUCache::addelement(const string &key, const string &value)
{
    _resultlist.push_front(make_pair(key, value));
    _pendingupdatelist.push_back(make_pair(key, value));
    _hashmap[key] = _resultlist.begin();
    _size++;
    if (_size > _capacity)
    {
        _hashmap.erase(_resultlist.back().first);
        _resultlist.pop_back();
    }
}
void LRUCache::readfromfile(const string &filename)
{
    unordered_map<string, list<pair<string, string>>::iterator> tmp_map;
    tmp_map.swap(_hashmap);
    list<pair<string, string>> tmp_res;
    tmp_res.swap(_resultlist);
    list<pair<string, string>> tmp_pending;
    tmp_pending.swap(_pendingupdatelist);
    ifstream ifs(filename);
    string line;
    while (std::getline(ifs, line))
    {
        istringstream iss(line);
        string key, value;
        iss >> key;
        iss >> value;
        _resultlist.push_back(make_pair(key, value));
        _hashmap[key] = _resultlist.end()--;
    }
}
void LRUCache::writetofile(const string &filename)
{
    ofstream ofs(filename);
    for (auto &elem : _resultlist)
    {
        ofs << elem.first << " "
            << elem.second << endl;
    }
}
void LRUCache::update(LRUCache &cache)
{
    list<pair<string, string>> tmp;
    tmp.swap(cache.getpendingupdatelist());
    for (auto &elem : tmp)
    {
        if (getelement(elem.first) == "")
        {
            addelement(elem.first, elem.second);
        }
    }
}
list<pair<string, string>> &LRUCache::getpendingupdatelist()
{
    return _pendingupdatelist;
}