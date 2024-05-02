#ifndef __THREAD_H__
#define __THREAD_H__

#include <pthread.h>
#include <functional>

namespace OurPool
{
    using std::bind;
    using std::function;

    class Thread
    {
        using ThreadCallback = function<void()>;

    public:
        Thread(ThreadCallback &&cb);
        ~Thread();

        // 线程运行与停止的函数
        void start();
        void join();

    private:
        // 线程入口函数
        static void *threadFunc(void *arg);

    private:
        pthread_t _thid;    // 线程id
        bool _isRunning;    // 标识线程是否在运行
        ThreadCallback _cb; // 就是需要执行的任务
    };

}; // end of OurPool

#endif
