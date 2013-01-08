/* 
 * File:   UdpSocket.h
 * Author: yzf
 *
 * Created on December 30, 2012, 3:49 PM
 */

#ifndef UDPSOCKET_H
#define UDPSOCKET_H

#include <string>

#define MAX_BUF_SIZE 1024//缓冲区大小
#define SLEEP_TIME 60

class UdpSocket {
public:
    UdpSocket();
    UdpSocket(int port);
    ~UdpSocket();
    int send(const char *ip, int port, char *data, int len);//发送数据
    void startRecving();//不新建线程接收数据，阻塞
    bool startRecvingAsyn();//新建线程接收数据，非阻塞
    bool sendHeartbeatAsyn();//新建线程发送心跳包，非阻塞
private:
    static void *_runRecving(void *obj);
    static void *_runHeartbeat(void *obj);
    void recvHandler(struct sockaddr_in *addr, char *data);//接收到udp包时的处理
private:
    static std::string _ip;
    static int _port;
    static std::string _content;
    
    int m_socket;
    bool m_running;
} ;


#endif // define UDPSOCKET_H
