#include "../../Include/ReactorV5/Thread.h"
#include "../../Include/ReactorV5/ThreadPool.h"
#include <unistd.h>

namespace OurPool
{
ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _queSize(queSize)
, _taskQue(_queSize)
, _isExit(false)
{
    _threads.reserve(_threadNum);
}

ThreadPool::~ThreadPool()
{

}

//线程池的启动与退出
void ThreadPool::start()
{
    //创建出所有的工作线程，并且将其放在vector中存储起来
    for(size_t idx = 0; idx < _threadNum; ++idx)
    {
        //线程池交给工作线程Thread做的任务，也就是doTask
        unique_ptr<Thread> up(new Thread(std::bind(&ThreadPool::doTask, this)));
        _threads.push_back(std::move(up));
    }

    //遍历vector，然后启动所有的工作线程（子线程）
    for(auto &th : _threads)
    {
        th->start();
    }
}

void ThreadPool::stop()
{
    //只要任务队列中的任务没有执行完毕，就
    //不能让工作线程退出
    while(!_taskQue.empty())
    {
        sleep(1);
    }

    _isExit = true;//将标志位设置为true
    _taskQue.wakeup();//唤醒所有等待在_notEmpty条件变量上的线程

    //遍历vector，然后停止所有的工作线程（子线程）
    for(auto &th : _threads)
    {
        th->join();
    }
}

//添加任务与获取任务
void ThreadPool::addTask(Task &&task)
{
    if(task)
    {
        _taskQue.push(std::move(task));
    }
}

Task ThreadPool::getTask()
{
    return _taskQue.pop();
}

//线程池交给每个具体线程做的任务，封装成了函数
void ThreadPool::doTask()
{
    //只要线程池不退出，就让工作线程一直去拿任务并且执行
    while(!_isExit)
    {
        //获取任务
        Task taskcb = getTask();
        if(taskcb)
        {
            //执行具体的任务
            taskcb();
        }
    }
}

};//end of OurPool
