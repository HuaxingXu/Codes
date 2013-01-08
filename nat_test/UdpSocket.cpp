#include <unistd.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "UdpSocket.h"

//std::string UdpSocket::_ip = "192.168.1.120";
std::string UdpSocket::_ip = "118.123.18.238";
int UdpSocket::_port = 22222;
std::string UdpSocket::_content = "heartbeat";

UdpSocket::UdpSocket() {
    this->m_running = false;
}
/*
 * port 要绑定的端口
 */
UdpSocket::UdpSocket(int port) {
    sockaddr_in localSin;
    bzero(&localSin, sizeof(localSin));
    localSin.sin_family = AF_INET;
    localSin.sin_port = htons(port);

    this->m_socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    int status = bind(this->m_socket, 
                (struct sockaddr *)&localSin, 
                sizeof(localSin));
    if (-1 == status) {
        printf("bind error\n");
        exit(1);
    }
    this->m_running = true;
}

UdpSocket::~UdpSocket() {
    if (true == this->m_running) {
        ::close(m_socket);
        this->m_running = false;
    }
}
/*
 * addr 接收方的地址结构体
 * data 数据
 * len 数据长度
 */
int UdpSocket::send(const char *ip, int port, char *data, int len) {
    printf("send data to %s:%d\n", ip, port);
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    return ::sendto(this->m_socket, data, len, 0, 
            (struct sockaddr *)&addr, sizeof(struct sockaddr));
}

void UdpSocket::startRecving() {
    this->_runRecving(this);
}

bool UdpSocket::startRecvingAsyn() {
    printf("receive data background\n");
    pthread_t pthreadId;
    if (pthread_create(&pthreadId, NULL, UdpSocket::_runRecving, this)) {
        return false;
    }
    return true;
}
/*
 * obj UdpSocket类型的指针
 */
void *UdpSocket::_runRecving(void* obj) {
    UdpSocket *udpSocket = (UdpSocket *)obj;
    
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(struct sockaddr_in);
    int errorNum;
    char data[MAX_BUF_SIZE];

    while (udpSocket->m_running) {
        bzero(&addr, sizeof(struct sockaddr_in));
        if((errorNum = recvfrom(udpSocket->m_socket, data, 
                    MAX_BUF_SIZE, 0,
                    (struct sockaddr*)&addr, &addrLen)) < 0) {
            printf("error num %d\n", errorNum);
            continue;
        }
        data[errorNum] = '\0';
        udpSocket->recvHandler(&addr, data);
    }
    return((void*)0);
}
/*
 * addr 地址结构体
 * data 数据
 */
void UdpSocket::recvHandler(sockaddr_in* addr, char* data) {
    printf("receive from: %s:%d    data: %s\n", 
            (const char*)inet_ntoa(addr->sin_addr), 
            ntohs(addr->sin_port), data);
}

bool UdpSocket::sendHeartbeatAsyn() {
    printf("send heart beat to %s:%d background\n", UdpSocket::_ip.c_str(), UdpSocket::_port);
    pthread_t pthreadId;
    if (pthread_create(&pthreadId, NULL, UdpSocket::_runHeartbeat, this)) {
        return false;
    }
    return true;
}

void *UdpSocket::_runHeartbeat(void* obj) {
    UdpSocket *udpSocket = (UdpSocket *)obj;
    
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(UdpSocket::_ip.c_str());
    addr.sin_port = htons(UdpSocket::_port);
    while (true) {
        ::sendto(udpSocket->m_socket, UdpSocket::_content.c_str(), 
                strlen(UdpSocket::_content.c_str()), 0, 
                (struct sockaddr *)&addr, sizeof(struct sockaddr));
        sleep(SLEEP_TIME);
    }
    return((void*)0);
}
