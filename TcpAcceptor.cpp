//
// Created by 裴沛东 on 2022/6/7.
//

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include "TcpAcceptor.h"
#include "Mycoroutine.h"

int TcpAcceptor::Accept() const {
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

void TcpAcceptor::ListenTCP(uint16_t port) {
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
    int val = 1;
    if(setsockopt(this->_fd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val)) < 0) {
        perror("reuse");
        exit(0);
    }
    if(bind(_fd,(struct sockaddr*)&addr,sizeof(addr)) < 0) {
        perror("bind");
        exit(0);
    }
    if(listen(_fd,32) < 0) {
        perror("listen");
        exit(0);
    }


}

TcpAcceptor::TcpAcceptor() {
    this->coroutine = Mycoroutine::mycoroutine();

}
