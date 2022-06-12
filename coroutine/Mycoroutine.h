//
// Created by 裴沛东 on 2022/6/7.
//
#pragma once
#ifndef COROUTINE_MYCOROUTINE_H
#define COROUTINE_MYCOROUTINE_H

#include <functional>
#include <ucontext.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <mutex>

class coroutine;
class Mycoroutine {
public:
    Mycoroutine();
    ~Mycoroutine();

    void co_create(std::function<void()> fun);   //添加任务进调度队列

    void co_dispatch();

    void co_yiled();

    void RegisterFdToScheduler(int fd,bool is_write);

    void SwitchToScheduler();

    void UnRegisterFdFromScheduler(int fd);

    ucontext_t* SchedCtx();

    static Mycoroutine* mycoroutine() {
        static thread_local Mycoroutine mycoroutine;
        return &mycoroutine;
    }




private:
    int epoll_fd_;
    coroutine* _cur_routine_;
    ucontext_t sched_ctx_;
    std::list<coroutine*> ready_lists_,running_lists_;
    typedef struct {
        coroutine *w_,*r_;
    }WaitingRoutines;
    std::unordered_map<int,WaitingRoutines> io_waiting_routines_;
    std::mutex mutex_;

};
#endif //COROUTINE_MYCOROUTINE_H
