#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include "StructFormat.h"

#define MAX_BUF_SIZE 1024 //缓冲区容量

using namespace std;

class ClientSocket
{
    public:
        ClientSocket();
        ClientSocket(int port);
        ~ClientSocket();
        bool startRecvingAsyn();//新建线程接收数据
        void startRecving();//不新建线程接收数据
        void send(char* data, struct sockaddr* addr);//发送数据
        void close();//关闭

    protected:

    private:
        static void* _runRecving(void *obj);//接收数据函数,是回调函数
        void recv_handler(char *data, struct sockaddr_in *addr);

        int socket;//套接字
        bool running;//运行状态
};

#endif // CLIENTSOCKET_H
