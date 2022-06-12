cmake_minimum_required(VERSION 3.16)
project(Coroutine)

set(CMAKE_CXX_STANDARD 14)

add_executable(Coroutine main.cpp coroutine/Mycoroutine.cpp coroutine/Mycoroutine.h coroutine/coroutine.cpp coroutine/coroutine.h net/co_socket.cpp net/co_socket.h net/TcpAcceptor.cpp net/TcpAcceptor.h net/TcpServer.cpp net/TcpServer.h net/ThreadPool.cpp net/ThreadPool.h net/Task.cpp net/Task.h net/TcpConnection.cpp net/TcpConnection.h net/TcpBuffer.cpp net/TcpBuffer.h)

target_link_libraries(Coroutine pthread)
