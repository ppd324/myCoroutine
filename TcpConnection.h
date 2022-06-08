//
// Created by 裴沛东 on 2022/6/8.
//

#ifndef COROUTINE_TCPCONNECTION_H
#define COROUTINE_TCPCONNECTION_H
#include <memory>
#include "Task.h"
#include "TcpBuffer.h"
class TcpConnection: public std::enable_shared_from_this<TcpConnection> {
public:
    TcpConnection(int buffSize,int fd);
    enum TcpConnectionState {
        Connected = 0,
        NotConnected = 1,
        HalfClosed = 2, // server call shutdown, write half close. can read,but can't write
        Closed =3
    };
    void ShutdownConnection();
    int Read();
    void Process();
    int Write();
    void Close();
    void InitBuffer(int bufferSize);
    void Clear();


private:
    int _fd{-1};
    std::shared_ptr<Task> _task;
    TcpConnectionState _sockState{TcpConnectionState::Connected};
    TcpBuffer::ptr _writeBuffer;
    TcpBuffer::ptr _readBuffer;







};


#endif //COROUTINE_TCPCONNECTION_H
