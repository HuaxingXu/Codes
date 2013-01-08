/* 
 * File:   UdpTransfer.h
 * Author: yzf
 *
 * Created on January 7, 2013, 9:43 AM
 */

#ifndef UDPTRANSFER_H
#define	UDPTRANSFER_H

#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>

#define UDP_MAX_SIZE 548//udp每次发送或接收的最大字节数
#define SLEEP_TIME 50000 //SLEEP_TIME = 1000000 为一秒

class UdpTransfer {
public:
    UdpTransfer(const char *target_ip, int target_port, int local_port);
    ~UdpTransfer();
    void resetTarget(const char *target_ip, int target_port);//重置对方地址
    int send(char data[], int size);//发送数据
    int recv(char recv_buffer[], int max_recv_size);//接收数据
private:
    sockaddr_in m_localSin;
    sockaddr_in m_targetSin;
    int m_socket;
};


#endif	/* UDPTRANSFER_H */

