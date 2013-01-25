#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "UdpTransfer.h"
#define DEBUG
#undef DEBUG
/*
 * const char *target_ip 对方ip
 * int target_port 对方端口
 * int local_port 本地端口
 */
UdpTransfer::UdpTransfer(const char *target_ip, int target_port, int local_port) {
    printf("UdpTransfer::UdpTransfer()\n");
    //本地
    bzero(&this->m_localSin, sizeof(this->m_localSin));
    this->m_localSin.sin_family = AF_INET;
    this->m_localSin.sin_port = htons(local_port);
    //对方
    bzero(&this->m_targetSin, sizeof(this->m_targetSin));
    this->m_targetSin.sin_family = AF_INET;
    this->m_targetSin.sin_addr.s_addr = inet_addr(target_ip);
    this->m_targetSin.sin_port = htons(target_port);
    //创建套接字
    this->m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    int bind_status = bind(this->m_socket,
                        (struct sockaddr *)&this->m_localSin, 
                        sizeof(this->m_localSin));
    if (bind_status == -1) {
        printf("bind error!\n");
        exit(-1);
    }
}

UdpTransfer::~UdpTransfer() {
    printf("UdpTransfer::~UdpTransfer()\n");
    close(this->m_socket);
}
/**
 * 重置目标的地址，跨网段时使用
 * const char *target_ip 对方的对外ip
 * int target_port 对方的对外端口
 */
void UdpTransfer::resetTarget(const char* target_ip, int target_port) {
//    printf("UdpTransfer::resetTarget()\n");
    this->m_targetSin.sin_addr.s_addr = inet_addr(target_ip);
    this->m_targetSin.sin_port = htons(target_port);
}
/*
 * 发送字节流
 * char data[] 要发送的字节流
 * int size 要发送的字节流的长度
 *
 * return int 成功发送的字节数
 */
unsigned int UdpTransfer::send(char data[], unsigned int size) {
//    printf("UdpTransfer::send()\n");
    if (size > UDP_MAX_SIZE) {
        size = UDP_MAX_SIZE;
    }

    socklen_t send_len = sizeof(struct sockaddr_in);
    char buffer[UDP_MAX_SIZE];
    unsigned int data_len;

    bzero(buffer, UDP_MAX_SIZE);
    memcpy(buffer, data, size);

//    char ip[50];
//    strcpy(ip, (const char*)inet_ntoa(this->m_targetSin.sin_addr));
//    printf("send to %s:%d\n", ip, ntohs(this->m_targetSin.sin_port));

    data_len = sendto(this->m_socket, buffer, size, 0,
            (struct sockaddr *)&this->m_targetSin, send_len);
#ifdef DEBUG
	printf("[UdpTransfer send]send data=%d bytes\n",data_len);
#endif
    usleep(SLEEP_TIME);
//    printf("send data: %d bytes\n", data_len);

    return data_len;
}
/*
 * 接收字节流
 * char recv_buffer[] 接收数据的缓冲区
 * int max_recv_size 缓冲区的长度
 *
 * return int 接收到的字节数
 */
unsigned int UdpTransfer::recv(char recv_buffer[], unsigned int max_recv_size) {
//    printf("UdpTransfer::recv()\n");
    if (max_recv_size > UDP_MAX_SIZE) {
        max_recv_size = UDP_MAX_SIZE;
    }

    unsigned int len;
    char buffer[UDP_MAX_SIZE];
    socklen_t recv_len = sizeof(struct sockaddr_in);
    //接受数据
    bzero(buffer, UDP_MAX_SIZE);
#ifdef DEBUG
	printf("[UdpTransfer recv]max_recv_size= %d\n",max_recv_size);
#endif
	
    len = recvfrom(this->m_socket, buffer, max_recv_size, 0, 
            (struct sockaddr *)&this->m_targetSin, &recv_len);
#ifdef DEBUG
	printf("[UdpTransfer recv]recv len= %d\n",len);
#endif

    char ip[50];
    strcpy(ip, (const char*)inet_ntoa(this->m_targetSin.sin_addr));
//    printf("recv from %s:%d\n", ip, ntohs(this->m_targetSin.sin_port));

    //保存收到的数据
    memcpy(recv_buffer, buffer, len * sizeof(char));

    return len;
}
