#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "ClientSocket.h"


class TCPSocket : public ClientSocket
{
    public:
        TCPSocket(int port);
        ~TCPSocket();
        bool connect(struct sockaddr* addr);//连接
        void send(char* data);//发送数据

    protected:

    private:
        int socket;//套接字
        bool running;//运行状态
        //void (*handler)(char*, struct sockaddr*);//对接收数据和地址进行处理的函数指针
};

#endif // TCPSOCKET_H
