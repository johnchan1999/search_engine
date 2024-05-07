#ifndef __CacheManager_H__
#define __CacheManager_H__
#include "LRUCache.h"
#include <vector>
class CacheManager
{
public:
    static CacheManager *createInstance();
    void initcache(size_t, const string &);
    LRUCache &getcache(size_t);
    void periodicupdatecaches();

private:
    CacheManager();
    ~CacheManager();

private:
    static CacheManager *_pInstance;
    vector<LRUCache &> _cachelist;
};
#endif