//
// Created by 裴沛东 on 2022/6/7.
//

#include "co_socket.h"
#include "coroutine.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

void Listener::ListenTCP(uint16_t port) {
    this->_fd = socket(AF_INET,SOCK_STREAM,0);
    if(_fd < 0) {
        perror("socket");
        exit(0);
    }
    if(fcntl(_fd,F_SETFL,O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(0);
    }
    struct sockaddr_in addr{};
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family = AF_INET;
    if(bind(_fd,(struct sockaddr*)&addr,sizeof(addr)) < 0) {
        perror("bind");
        exit(0);
    }
    if(listen(_fd,32) < 0) {
        perror("listen");
        exit(0);
    }
    printf("listen success\n");


}

int Listener::Accept() {
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    while(true) {
        int client_fd = accept(_fd,nullptr,nullptr);
        if(client_fd > 0) {
            if(fcntl(client_fd,F_SETFL,O_NONBLOCK) < 0) {
                perror("fcntl");
                exit(0);
            }
            return client_fd;
        }
        else if(errno == EAGAIN) { //非阻塞，说明事件未到达，可以放弃占用cpu
            //此时应该让出cpu，重新调度别的协程
            /*1.注册当前fd到调度器
              2.切换到调度器
            */
            coroutine->RegisterFdToScheduler(_fd,false);
            coroutine->SwitchToScheduler();

        }else if(errno == EINTR) {
            continue;
        }else {
            perror("accept");
            exit(0);
        }

    }

}

Connection::Connection(int fd):_fd(fd) {}
void Connection::Close() {
    if(_fd < 0) {
        return;
    }
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    coroutine->UnRegisterFdFromScheduler(_fd);
    close(_fd);

}

int Connection::Read(char* buf,size_t size) {
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    while(true) {
        int ret = read(_fd,buf,size);
        if(ret > 0) {
            return ret;
        }else if(ret == 0) {
            return 0;
        }else if(errno != EAGAIN && errno != EINTR) {
            perror("read");
            return -1;
        }else if(errno == EAGAIN) {
            printf("read fd %d yeild\n",_fd);
            coroutine->RegisterFdToScheduler(_fd,false);
            coroutine->SwitchToScheduler();
        }

    }
    return -1;
}

int Connection::Write(const char* buf, size_t size) {
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    size_t write_bytes = 0;

    while(write_bytes < size) {
        int ret = write(_fd,buf+write_bytes,size - write_bytes);
        if(ret > 0) {
            write_bytes += ret;
        }else if(ret == 0) {
            return 0;
        }else if(errno != EAGAIN && errno != EINTR) {
            perror("write");
            return -1;
        }else if(errno == EAGAIN) {
            printf("write fd %d yeild\n",_fd);
            coroutine->RegisterFdToScheduler(_fd,true);
            coroutine->SwitchToScheduler();
        }

    }
    return size;

}

