#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "NonCopyable.h"
#include <pthread.h>

namespace OurPool
{
    class MutexLock;

    class Condition
        : public NonCopyable
    {
    public:
        ~Condition();
        Condition(MutexLock &mutex);
        void notifyAll(); // 广播：唤醒所有沉睡的线程
        void notify();    // 单播：唤醒最少一个沉睡的线程
        void wait();

    private:
        MutexLock &_mutex; // 自解锁
        pthread_cond_t _cond;
    };
}; // end of OurPool

#endif
