//
// Created by 裴沛东 on 2022/6/8.
//

#ifndef COROUTINE_TCPBUFFER_H
#define COROUTINE_TCPBUFFER_H


#include <vector>
#include <memory>

class TcpBuffer {
public:
    typedef std::shared_ptr<TcpBuffer> ptr;
    explicit TcpBuffer(int size);
    int Readable() const;
    int Writeable() const;
    int ReadIndex() const;
    int WriteIndex() const;
    void WriteToBuffer(const char* buf,size_t size);
    int Size() const;
    void ClearBuffer();
    void ResizeBuffer(size_t size);
    std::string BufferToString();
    void recycleWrite(int size);
    void AdjustBuffer();
    void recycleRead(int size);
    void resetBuffer();


private:
    int _readIndex{0};
    int _writeIndex{0};
    int _size{0};
public:
    std::vector<char> _buffer;

};


#endif //COROUTINE_TCPBUFFER_H
