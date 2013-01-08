#include "UDPSocket.h"

UDPSocket::UDPSocket()
{

}
//直接调用父类的构造函数
UDPSocket::UDPSocket(int port) : ClientSocket(port)
{

}

UDPSocket::~UDPSocket()
{
    //dtor
}
