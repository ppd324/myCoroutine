cmake_minimum_required(VERSION 3.16)
project(Coroutine)

set(CMAKE_CXX_STANDARD 14)

add_executable(Coroutine main.cpp Mycoroutine.cpp Mycoroutine.h coroutine.cpp coroutine.h co_socket.cpp co_socket.h)
