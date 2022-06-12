//
// Created by 裴沛东 on 2022/6/7.
//

#include "coroutine.h"
#include <sys/epoll.h>
class coroutine;
Mycoroutine::Mycoroutine() {
    _cur_routine_ = nullptr;
    epoll_fd_ = epoll_create1(0);
    if(epoll_fd_ < 0) {
        perror("epoll create");
        exit(0);
    }

}
Mycoroutine::~Mycoroutine() {

}

void Mycoroutine::co_create(std::function<void()> func) {
    coroutine *routine = new coroutine(func,this);
    std::lock_guard<std::mutex> lock(mutex_);
    ready_lists_.emplace_back(routine);

}
void Mycoroutine::co_yiled() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ready_lists_.push_back(_cur_routine_);
    }
    swapcontext(_cur_routine_->Ctx(),&sched_ctx_);

}

void Mycoroutine::co_dispatch() {
    struct epoll_event events[128];
    while (true)
    {
        // if(ready_lists_.size() == 0) {
        //     continue;
        // }
        {
            std::lock_guard<std::mutex> lock(mutex_);
            running_lists_ = std::move(ready_lists_);
        }
        ready_lists_.clear();
        for(auto it = running_lists_.begin();it != running_lists_.end();++it) {
            _cur_routine_ = *it;
            swapcontext(&sched_ctx_,(*it)->Ctx());
            _cur_routine_ = nullptr;
            if((*it)->finished()) {
                delete *it;

            }
        }
        running_lists_.clear();
        int readyNum = epoll_wait(epoll_fd_,events,128,5);
        if(readyNum < 0) {
            perror("epoll_wait");
            exit(0);
        }
        for(int i = 0;i<readyNum;++i) {
            auto &event = events[i];
            int fd = event.data.fd;
            if(io_waiting_routines_.count(fd) > 0) {
                //唤醒对应协程
                printf("Epoll wake up fd\n");
                if(event.events | EPOLLIN && io_waiting_routines_[fd].r_) {
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        ready_lists_.push_back(io_waiting_routines_[fd].r_);
                    }
                }
                if(event.events | EPOLLOUT && io_waiting_routines_[fd].w_) {
                    {
                        std::lock_guard<std::mutex> lock(mutex_);
                        ready_lists_.push_back(io_waiting_routines_[fd].w_);
                    }
                }

            }else {
                continue;
            }
        }

    }


}

void Mycoroutine::RegisterFdToScheduler(int fd,bool is_write) {
    if(io_waiting_routines_.count(fd) == 0) {
        //未注册
        WaitingRoutines wr;
        if(!is_write) {
            wr.r_ = _cur_routine_;
            wr.w_ = nullptr;
        }else {
            wr.w_ = _cur_routine_;
            wr.r_ = nullptr;
        }
        io_waiting_routines_[fd] = wr;
        //将fd注册到epoll
        struct epoll_event ev;
        ev.data.fd = fd;
        ev.events = is_write ? EPOLLOUT | EPOLLET : EPOLLIN | EPOLLET;
        if(epoll_ctl(epoll_fd_,EPOLL_CTL_ADD,fd,&ev) < 0) {
            perror("epoll_ctl add");
            exit(0);
        }
    }else {
        if(!is_write) {
            io_waiting_routines_[fd].r_ = _cur_routine_;
        }else {
            io_waiting_routines_[fd].w_ = _cur_routine_;
        }
    }
}
void Mycoroutine::UnRegisterFdFromScheduler(int fd) {
    if(io_waiting_routines_.count(fd) == 0) {
        return;
    }
    if(epoll_ctl(epoll_fd_,EPOLL_CTL_DEL,fd,nullptr) < 0) {
        perror("epoll del");
        exit(0);
    }
    io_waiting_routines_.erase(fd);
}
void Mycoroutine::SwitchToScheduler() {
    swapcontext(_cur_routine_->Ctx(),&sched_ctx_);

}

ucontext_t* Mycoroutine::SchedCtx() {
    return &this->sched_ctx_;
}

