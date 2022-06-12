//
// Created by 裴沛东 on 2022/6/7.
//

#ifndef COROUTINE_TCPSERVER_H
#define COROUTINE_TCPSERVER_H

#include <memory>
#include "Task.h"
#include "ThreadPool.h"
#include "TcpAcceptor.h"
#include "../coroutine/Mycoroutine.h"

class TcpServer {
public:
    explicit TcpServer(uint16_t port = 8000,uint8_t num = 5);
    void echo(int fd);

private:
    std::shared_ptr<TcpAcceptor> _accept;
    std::shared_ptr<ThreadPool> _threadPool;
    std::vector<std::shared_ptr<Task>> _subTasks;
    Mycoroutine* _coroutine;
    uint16_t _port;
    uint8_t _threadNum;
    bool IsClose;


};


#endif //COROUTINE_TCPSERVER_H
