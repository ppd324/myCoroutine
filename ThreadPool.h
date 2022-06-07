//
// Created by 裴沛东 on 2022/6/7.
//

#ifndef COROUTINE_THREADPOOL_H
#define COROUTINE_THREADPOOL_H

#include <thread>
#include <vector>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    explicit ThreadPool(uint num = std::thread::hardware_concurrency());

    bool AddTask(const std::function<void()>& fun);

    void Close();

    ~ThreadPool();

private:
    uint _ThreadNum;
    std::vector<std::thread> _threadArray;
    std::queue<std::function<void()>> _taskQue;
    std::mutex _mutex;
    std::condition_variable _cond;
    bool _close;



};


#endif //COROUTINE_THREADPOOL_H
