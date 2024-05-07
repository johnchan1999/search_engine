#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>
#include <string>
#include "../LRUcache/CacheManager.h"
using std::string;

namespace OurPool
{
    namespace current_thread
    {

        extern __thread const char *_name;
    }
    // namespacOurPoole

    using std::bind;
    using std::function;

    class Thread
    {
        using ThreadCallback = function<void()>;

    public:
        Thread(ThreadCallback &&cb, string name);
        ~Thread();

        // 线程运行与停止的函数
        void start();
        void join();

    private:
        // 线程入口函数
        static void *threadFunc(void *arg);

    private:
        pthread_t _thid; // 线程id
        string _name;
        bool _isRunning;    // 标识线程是否在运行
        ThreadCallback _cb; // 就是需要执行的任务
        LRUCache _cache;
    };
};
#endif