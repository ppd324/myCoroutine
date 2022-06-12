//
// Created by 裴沛东 on 2022/6/7.
//

#ifndef COROUTINE_CO_SOCKET_H
#define COROUTINE_CO_SOCKET_H


#include <stdint.h>
#include <stdlib.h>
class Listener {
public:
    void ListenTCP(uint16_t port);

    int Accept();
private:
    int _fd;


};

class Connection {
public:
    Connection(int fd);
    void Close();
    int Read(char* buf,size_t size);
    int Write(const char* buf,size_t size);
private:
    int _fd;


};


#endif //COROUTINE_CO_SOCKET_H
