#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include "ClientSocket.h"


class UDPSocket : public ClientSocket
{
    public:
        UDPSocket();
        UDPSocket(int port);
        ~UDPSocket();
    protected:
    private:
};

#endif // UDPSOCKET_H
