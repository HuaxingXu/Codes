#include <cstdio>
#include <cstring>
#include <iostream>

#include "UdpSocket.h"

using namespace std;

char hello[6] = "hello";

int main() {
    
    int port;
    printf("input port you want to use: ");
    scanf("%d", &port);
    UdpSocket *udpSocket = new UdpSocket(port);
    udpSocket->startRecvingAsyn();
    udpSocket->sendHeartbeatAsyn();
    
    int cmd;
    char targetIp[20];
    int targetPort;
    while (true) {
        scanf("%d", &cmd);
        if (1 == cmd) {
            scanf("%s%d", targetIp, &targetPort);
            udpSocket->send(targetIp, targetPort, hello, 5);
        }
        else {
            break;
        }
    }
    
    delete udpSocket;
    
    return 0;
}
