//
// Created by 裴沛东 on 2022/6/7.
//

#ifndef COROUTINE_TCPACCEPTOR_H
#define COROUTINE_TCPACCEPTOR_H


#include <cstdint>
#include "Mycoroutine.h"
class TcpAcceptor {
public:
    TcpAcceptor();
    void ListenTCP(uint16_t port);
    int Accept() const;
private:
    int _fd;
    Mycoroutine *coroutine;


};


#endif //COROUTINE_TCPACCEPTOR_H
