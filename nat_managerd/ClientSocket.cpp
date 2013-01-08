#include "ClientSocket.h"
#include <vector>
#include "Manager.h"

ClientSocket::ClientSocket()
{

}
/*
***构造函数
＊*port端口号
*/
ClientSocket::ClientSocket(int port)
{
    this->socket = ::socket(AF_INET, SOCK_DGRAM, 0);

    if (port >1024)
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(struct sockaddr_in));
        socklen_t addr_len = sizeof(struct sockaddr_in);
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = Manager::local_ip;
        addr.sin_port = htons(port);

        if (bind(this->socket, (struct sockaddr*)&addr, addr_len))
        {
            cout << "ClientSocket绑定UDP端口" << port << "失败！" << endl;
            exit(EXIT_FAILURE);
        }
    }

    this->running = true;
}

ClientSocket::~ClientSocket()
{
    //dtor
}

void* ClientSocket::_runRecving(void *obj)
{
    ClientSocket *p = (ClientSocket*) obj;//当前ClientSocket对象指针

    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    int error_num;
    char data[MAX_BUF_SIZE];

    while (p->running)
    {
        memset(&addr, 0, sizeof(struct sockaddr_in));
        if((error_num = recvfrom(p->socket, data, MAX_BUF_SIZE, 0,
                                 (struct sockaddr*)&addr, &addr_len) < 0))
        {
            //将异常写入日志队列
            char message[256];
            snprintf(message, 256, "接收数据出错:%s", strerror(error_num));
            Manager::log.record(string(message));

            continue;
        }

        p->recv_handler(data, &addr);
    }
    return((void*)0);
}

bool ClientSocket::startRecvingAsyn()
{
    pthread_t ntid;//新线程ID

    if (pthread_create(&ntid, NULL, ClientSocket::_runRecving, this))
    {
        return false;
    }

    return true;
}

void ClientSocket::startRecving()
{
    this->_runRecving(this);
}

void ClientSocket::send(char *data, struct sockaddr *addr)
{
    sendto(this->socket,data,sizeof(data),0,addr,sizeof(struct sockaddr));
}

void ClientSocket::close()
{
    this->running = false;
    ::close(this->socket);
}

//接收UDP包时的处理函数
void ClientSocket::recv_handler(char *data, struct sockaddr_in *addr)
{
    //提取操作码
    char op;
    memcpy(&op, data, sizeof(char));

    //当消息为登记客户端收发命令的UDP地址
    if (op == REGIST_CMD_ADDR)
    {
        struct regist_addr recv_data;

        unpack_regist_addr(&recv_data, data);

        string client_name = string(recv_data.client_name);

        //机器人已经验证通过则允许下面的操作
        if (Manager::logined_clients.count(client_name))
        {
            struct sockaddr_in *recv_addr = (struct sockaddr_in*) addr;
            unsigned int addr_ip = ntohl(recv_addr->sin_addr.s_addr);
            unsigned int addr_port = ntohs(recv_addr->sin_port);

            vector<unsigned int> client_addr(2,0);
            client_addr[0] = addr_ip;
            client_addr[1] = addr_port;

            if (Manager::mysqlDao.registAddrByName(client_name, client_addr, CMD_PORT))
            {
                string client_type = "机器人PID";

                if (client_name.rfind("_mobile") != string::npos)
                {
                    client_type = "手机用户";
                }

                string format = "%s %s 注册命令地址成功 访问来自 %s:%u";
                char message[256];
                snprintf(message, 256, format.c_str(), client_type.c_str(), client_name.c_str(),
                         inet_ntoa(recv_addr->sin_addr), ntohs(recv_addr->sin_port));
                Manager::log.record(string(message));
            }
        }
    }

    //当消息为登记客户端收发语音命令的UDP地址
    else if (op == REGIST_VOICE_ADDR)
    {
        struct regist_addr recv_data;

        unpack_regist_addr(&recv_data, data);

        string client_name = string(recv_data.client_name);

        //机器人已经验证通过则允许下面的操作
        if (Manager::logined_clients.count(client_name))
        {
            struct sockaddr_in *recv_addr = (struct sockaddr_in*) addr;
            unsigned int addr_ip = ntohl(recv_addr->sin_addr.s_addr);
            unsigned int addr_port = ntohs(recv_addr->sin_port);

            vector<unsigned int> client_addr(2,0);
            client_addr[0] = addr_ip;
            client_addr[1] = addr_port;

            if (Manager::mysqlDao.registAddrByName(client_name, client_addr, VOICE_PORT))
            {
                string client_type = "机器人PID";

                if (client_name.rfind("_mobile") != string::npos)
                {
                    client_type = "手机用户";
                }

                string format = "%s %s 注册语音地址成功 访问来自 %s:%u";
                char message[256];
                snprintf(message, 256, format.c_str(), client_type.c_str(), client_name.c_str(),
                         inet_ntoa(recv_addr->sin_addr), ntohs(recv_addr->sin_port));
                Manager::log.record(string(message));
            }
        }
    }
}


