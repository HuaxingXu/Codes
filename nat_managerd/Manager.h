#ifndef MANAGER_H
#define MANAGER_H

#include "MySQLDao.h"
#include "ClientSocket.h"
#include "Operation.h"
#include "TCPSocket.h"
#include "TcpServerSocket.h"
#include "UDPSocket.h"
#include "Log.h"
#include <map>
#include <vector>
#include <stdlib.h>
#include<openssl/rsa.h>
#include <set>

//typedef map<string,string> hashmap;//声明一个哈希映射

class Manager
{
    public:
        static map<string,int> client_socket;//client<-->socket映射
        static map<int,string> socket_client;//socket<-->client
        static map<string, RSA*> client_pk;//产品ID<-->密钥映射
        static set<string> logined_clients;//已成功登录的客户端集合
        static MySQLDao mysqlDao;
        static UDPSocket udp_server;
        static TcpServerSocket tcp_server;
        static Log log;
        static in_addr_t local_ip;

        Manager();
        ~Manager();
        static void start();//启动服务进程
        static void end();//结束服务进程

    protected:

    private:
};

#endif // MANAGER_H
