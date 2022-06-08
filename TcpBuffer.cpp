//
// Created by 裴沛东 on 2022/6/8.
//

#include <cstring>
#include "TcpBuffer.h"

TcpBuffer::TcpBuffer(int size):_size(size) {
    _buffer.resize(size);
}

int TcpBuffer::Readable() const {
    return _writeIndex - _readIndex;
}

int TcpBuffer::Writeable() const {
    return _buffer.size() - _writeIndex;
}

int TcpBuffer::ReadIndex() const {
    return _readIndex;
}

int TcpBuffer::WriteIndex() const {
    return _writeIndex;
}

void TcpBuffer::WriteToBuffer(const char *buf, size_t size) {
    if(size > Writeable()) {
        int new_size = (int)(1.5 * (_writeIndex + size));
        ResizeBuffer(new_size);
    }
    memcpy(&_buffer[_writeIndex],buf,size);
    _writeIndex += size;

}

int TcpBuffer::Size() const {
    return _buffer.size();
}

void TcpBuffer::ClearBuffer() {
    _buffer.clear();
    _writeIndex = 0;
    _readIndex = 0;
}

void TcpBuffer::ResizeBuffer(size_t size) {
    std::vector<char> tmp(size);
    int c = std::min((int)size, Readable());
    memcpy(&tmp[0], &_buffer[_readIndex], c);

    _buffer.swap(tmp);
    _readIndex = 0;
    _writeIndex = _readIndex + c;

}

std::string TcpBuffer::BufferToString() {
    return std::string{_buffer.begin(),_buffer.end()};
}

void TcpBuffer::recycleWrite(int size) {
    int j = _writeIndex + size;
    if(j > _buffer.size()) {
        perror("recycle Write");
        exit(0);
    }
    _writeIndex = j;
    AdjustBuffer();
}

void TcpBuffer::AdjustBuffer() {
    if(_readIndex > _buffer.size() / 3) {
        std::vector<char> new_buffer(_buffer.size());
        int count = Readable();
        memcpy(&new_buffer[0],&_buffer[_readIndex],count);
        _buffer.swap(new_buffer);
        _writeIndex = count;
        _readIndex = 0;
        new_buffer.clear();
    }

}

void TcpBuffer::recycleRead(int size) {
    int j = _readIndex + size;
    if(j > _buffer.size()) {
        perror("recycle read");
        exit(0);
    }
    _readIndex = j;
    AdjustBuffer();

}

void TcpBuffer::resetBuffer() {
    _readIndex = 0;
    _writeIndex = 0;

}
