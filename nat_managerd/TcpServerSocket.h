#ifndef TCPSERVERSOCKET_H
#define TCPSERVERSOCKET_H

//#pragma pack(1)//字节对齐方式为1个字节

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
#include <sys/ioctl.h>
#include <net/if.h>
#include "RSAEncry.h"

#define MAX_EPOLL_FD_NUM 1024  //epoll所支持的最大句柄数
#define MAX_BUF_SIZE 1024 //缓冲区容量

using namespace std;

class TcpServerSocket
{
    public:
        TcpServerSocket();
        TcpServerSocket(int port);
        ~TcpServerSocket();
        bool startRecvingAsyn();//使用新线程做接收
        void startRecving();//不使用新线程做接收
        void close();//关闭套接字
        static in_addr_t get_local_ip();//获取本地IP地址
        static void setnonblocking(int socket);// 将新连接置于非阻塞模式

    protected:

    private:
        static void* _runListening(void* obj);//监听socket，在这里使用了epoll模型,是回调函数

        void recv_handler(int socket, char *data);
        void tcp_except(int socket);

        bool running;//运行状态
        int server_socket;//服务器套接字
        int epollfd;//epoll句柄
};

#endif // TCPSERVERSOCKET_H
