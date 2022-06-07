//
// Created by 裴沛东 on 2022/6/7.
//

#include <cstring>
#include "TcpServer.h"
#include "Task.h"
#include "co_socket.h"
TcpServer::TcpServer(uint16_t port,uint8_t num):_port(port),_threadNum(num) {
    _accept = std::make_shared<TcpAcceptor>();
    _threadPool = std::make_shared<ThreadPool>(_threadNum);
    for(int i = 0;i<_threadNum;++i) {
        std::shared_ptr<Task> t = std::make_shared<Task>();
        _subTasks.emplace_back(t);
        std::function<void()> fun = std::bind(&Task::run,t.get());
        _threadPool->AddTask(fun);
    }
    _accept->ListenTCP(8000);
    _coroutine = Mycoroutine::mycoroutine();
    _coroutine->co_create([&](){
        while(true) {
            int fd = _accept->Accept();
            //将客户端连接加入subReactor
            int rand = fd % _threadNum;
            _subTasks[rand]->addTask(std::bind(&TcpServer::echo, this, fd));
        }

    });
    _coroutine->co_dispatch();
}

void TcpServer::echo(int fd) {
    Connection conn(fd);
    char buf[128] = {0};
    while(true) {
        memset(buf,0,128);
        int ret = conn.Read(buf,128);
        if(ret <= 0) {
            conn.Close();
            break;
        }
        ret = conn.Write(buf,ret);
        if(ret <= 0) {
            conn.Close();
            break;
        }

    }



}

