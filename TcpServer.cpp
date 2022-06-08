//
// Created by 裴沛东 on 2022/6/7.
//

#include <cstring>
#include "TcpServer.h"
#include "Task.h"
#include "TcpConnection.h"
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
    TcpConnection conn(128,fd);
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    std::cout<<"thread id is:"<<std::this_thread::get_id()<<" coroutine is:"<<coroutine<<std::endl;
    while(true) {
        int ret = conn.Read();
        if(ret <= 0) {
            conn.Close();
            break;
        }
        conn.Process();
        ret = conn.Write();
        if(ret <= 0) {
            conn.Close();
            break;
        }
        conn.Clear();

    }



}

