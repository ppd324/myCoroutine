//
// Created by 裴沛东 on 2022/6/7.
//

#include "coroutine.h"
coroutine::coroutine(std::function<void()> fun,Mycoroutine *mycoroutine) {
    this->fun = fun;
    this->status = 0;
    getcontext(&(this->_ctx));
    this->stack_size_ = 1024*128;
    this->stack_ = new char[stack_size_];
    this->_ctx.uc_link = mycoroutine->SchedCtx();
    this->_ctx.uc_stack.ss_sp = stack_;
    this->_ctx.uc_stack.ss_size = stack_size_;

    makecontext(&_ctx,(void(*)())coroutine::run,1,this);

}

coroutine::~coroutine() {
    delete stack_;
    stack_ = nullptr;
    stack_size_ = 0;

}
void coroutine::run(coroutine *co) {
    co->fun();
    co->status = -1;
}
ucontext_t* coroutine::Ctx() {
    return &_ctx;
}

bool coroutine::finished() {
    return status == -1;

}
