#include "Manager.h"

in_addr_t Manager::local_ip = TcpServerSocket::get_local_ip();//获取本地IP地址

//建立数据库操作类
MySQLDao Manager::mysqlDao = MySQLDao();
//创建UDP Server，绑定端口11110，其实只是一个UDPsocket而已
UDPSocket Manager::udp_server = UDPSocket(11110);
//创建TCP Server, 绑定端口11111
TcpServerSocket Manager::tcp_server = TcpServerSocket(11111);

//创建日志实例
Log Manager::log = Log();

map<string,int> Manager::client_socket;
map<int,string> Manager::socket_client;
map<string, RSA*> Manager::client_pk;
set<string> Manager::logined_clients;

Manager::Manager()
{

}

Manager::~Manager()
{
    //dtor
}

//开启服务
void Manager::start()
{
    if (!Manager::mysqlDao.connect())
    {
        cout << "连接数据库失败" << endl;
        exit(EXIT_FAILURE);
    }

    if (!Manager::tcp_server.startRecvingAsyn())
    {
        cout << "创建tcp_server线程失败" << endl;
        exit(EXIT_FAILURE);
    }

    if (!Manager::udp_server.startRecvingAsyn())
    {
        cout << "创建udp_server线程失败" << endl;
        exit(EXIT_FAILURE);
    }
}

//关闭服务
void Manager::end()
{
    Manager::tcp_server.close();
    Manager::udp_server.close();
    Manager::mysqlDao.close();
}
