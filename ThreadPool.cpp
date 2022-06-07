//
// Created by 裴沛东 on 2022/6/7.
//

#include "ThreadPool.h"

bool ThreadPool::AddTask(const std::function<void()> &fun) {
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _taskQue.push(fun);
    }
    _cond.notify_one();
}

ThreadPool::ThreadPool(uint num):_ThreadNum(num) {
    for(int i = 0;i<num;++i) {
        _threadArray.emplace_back([&]() {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    while(_taskQue.empty()) {
                        _cond.wait(lock);
                    }
                    task = std::move(_taskQue.front());
                    _taskQue.pop();
                }
                task();
                if(_close) break;
            }
        });
    }

}

void ThreadPool::Close() {
    _close = true;


}

ThreadPool::~ThreadPool() {
    for(auto &t : _threadArray) {
        t.join();
    }

}
