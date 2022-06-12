//
// Created by 裴沛东 on 2022/6/7.
//

#ifndef COROUTINE_COROUTINE_H
#define COROUTINE_COROUTINE_H


#include "Mycoroutine.h"

class coroutine {
public:
    coroutine(std::function<void()> fun,Mycoroutine *mycoroutine);
    ~coroutine();
    ucontext_t* Ctx();
    bool finished();
    static void run(coroutine *co);
private:
    int status;
    ucontext_t _ctx;
    std::function<void()> fun;

    char *stack_;

    size_t stack_size_;



};



#endif //COROUTINE_COROUTINE_H
