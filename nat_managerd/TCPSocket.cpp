#include "TCPSocket.h"

TCPSocket::TCPSocket(int port)
{
    //this->handler = NULL;
    this->socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (port >1024)
    {
        struct sockaddr_in localAddr;
        bzero(&localAddr, sizeof(localAddr));
        localAddr.sin_family = AF_INET;
        localAddr.sin_port = htons(port);
        localAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(this->socket, (struct sockaddr*) &localAddr, sizeof(localAddr)))
        {
            cout << "TCPSocket绑定TCP端口" << port << "失败！" << endl;
            exit(1);
        }
    }

    this->running = false;
}

TCPSocket::~TCPSocket()
{
    //dtor
}

bool TCPSocket::connect(struct sockaddr *addr)
{
    if (!(::connect(this->socket, addr, sizeof(addr))))
    {
        this->running = true;
        return true;
    }
    else
    {
        return false;
    }
}

void TCPSocket::send(char* data)
{
    ::send(this->socket,data,sizeof(data),0);
}
