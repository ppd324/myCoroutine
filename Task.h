//
// Created by 裴沛东 on 2022/6/7.
//

#ifndef COROUTINE_TASK_H
#define COROUTINE_TASK_H
#include <utility>

#include "Mycoroutine.h"

class Task {
public:
    void run() {
        coroutine = Mycoroutine::mycoroutine();
        coroutine->co_dispatch();
    }
    void addTask(std::function<void()> fun) {
        coroutine->co_create(std::move(fun));
    }

private:
    Mycoroutine *coroutine;

};


#endif //COROUTINE_TASK_H
