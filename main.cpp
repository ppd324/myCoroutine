#include "coroutine/coroutine.h"
#include "net/co_socket.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include "net/TcpServer.h"

int main() {
//    Mycoroutine *mycoroutine = Mycoroutine::mycoroutine();
//    mycoroutine->co_create([&]() {
//        Listener listener;
//        listener.ListenTCP(8888);
//        while(true) {
//            int client_fd = listener.Accept();
//            if(client_fd < 0) {
//                continue;
//            }
//            printf("client connected\n");
//            Connection conn(client_fd);
//            mycoroutine->co_create([&]() {
//                char buf[128] = {0};
//                while(true) {
//                    memset(buf,0,128);
//                    int n = conn.Read(buf,128);
//                    if(n <= 0) {
//                        printf("close fd\n");
//                        conn.Close();
//                        break;
//                    }
//                    printf("recv: %s",buf);
//                    n = conn.Write(buf,n);
//                    if(n <= 0) {
//                        printf("close fd\n");
//                        conn.Close();
//                        break;
//                    }
//                }
//            });
//
//        }
//    });
//    mycoroutine->co_dispatch();
    TcpServer tcpServer(8000,1);


    return 0;
}