//
// Created by 裴沛东 on 2022/6/8.
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
#include "TcpConnection.h"

void TcpConnection::ShutdownConnection() {
    if(_sockState == TcpConnectionState::Closed || _sockState == TcpConnectionState::NotConnected) {
        return;
    }
    _sockState = TcpConnectionState::HalfClosed;
    shutdown(_fd,SHUT_RDWR);

}

TcpConnection::TcpConnection(int bufferSize,int fd):_fd(fd) {
    if(fcntl(_fd,F_SETFL,O_NONBLOCK) < 0) {
        perror("fcntl");
        exit(0);
    }
    InitBuffer(bufferSize);
}

int TcpConnection::Read() {
        Mycoroutine *coroutine = Mycoroutine::mycoroutine();
        bool readall = false;
        int readsize = 0;
        while(!readall) {
            if(_readBuffer->Writeable() == 0) {
                _readBuffer->ResizeBuffer(2*_readBuffer->Size());
            }
            int readCount = _readBuffer->Writeable();
            printf("readcount is %d\n",readCount);
            int writeIndex = _readBuffer->WriteIndex();
            int ret = read(_fd,&_readBuffer->_buffer[writeIndex],readCount);
            if(ret > 0 && ret == readCount) {
                printf("read %d bytes\n",ret);
                _readBuffer->recycleWrite(ret);
                readsize += ret;
                continue;
            }else if(ret > 0 && ret < readCount) {
                printf("read %d bytes return\n",ret);
                _readBuffer->recycleWrite(ret);
                readall = true;
                readsize += ret;
                return readsize;
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

int TcpConnection::Write() {
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    size_t write_bytes = 0;
    while(write_bytes < _writeBuffer->Size()) {
        if(_writeBuffer->Readable() == 0) {
            break;
        }
        int writeSize = _writeBuffer->Readable();
        int readIndex = _writeBuffer->ReadIndex();
        int ret = write(_fd,&_writeBuffer->_buffer[readIndex],writeSize);
        if(ret > 0) {
            write_bytes += ret;
            _writeBuffer->recycleRead(ret);
            if(_writeBuffer->Readable() <= 0) {  //数据全部发送完毕
                break;
            }
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
    return write_bytes;
}

void TcpConnection::Close() {
    if(_fd < 0) {
        return;
    }
    Mycoroutine *coroutine = Mycoroutine::mycoroutine();
    coroutine->UnRegisterFdFromScheduler(_fd);
    close(_fd);

}

void TcpConnection::InitBuffer(int bufferSize) {
    _readBuffer = std::make_shared<TcpBuffer>(bufferSize);
    _writeBuffer = std::make_shared<TcpBuffer>(bufferSize);

}

void TcpConnection::Process() {
    _writeBuffer->WriteToBuffer(&_readBuffer->_buffer[0],_readBuffer->Readable());
}

void TcpConnection::Clear() {
    _writeBuffer->resetBuffer();
    _readBuffer->resetBuffer();

}


