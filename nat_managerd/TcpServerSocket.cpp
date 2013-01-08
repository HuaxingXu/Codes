#include "TcpServerSocket.h"
#include "Manager.h"

//获取本地IP地址
in_addr_t TcpServerSocket::get_local_ip()
{
    in_addr_t local_ip;
    int sockfd;
    struct ifreq ifr;
    struct sockaddr_in *saddr;

    if ((sockfd=socket(PF_INET,SOCK_DGRAM,0)) < 0)
    {
        local_ip = 0;
    }

    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "eth0");
    ifr.ifr_addr.sa_family = AF_INET;

    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0)
    {
        local_ip = 0;
    }

    saddr = (struct sockaddr_in*)&ifr.ifr_addr;
    local_ip = saddr->sin_addr.s_addr;

    return local_ip;
}

// 将新连接置于非阻塞模式
void TcpServerSocket::setnonblocking(int socket)
{
    int opts;
    opts = fcntl(socket, F_GETFL);
    opts |= O_NONBLOCK;
    fcntl(socket, F_SETFL, opts);
}

TcpServerSocket::TcpServerSocket()
{

}

TcpServerSocket::TcpServerSocket(int port)
{
    this->running = false;

    this->server_socket = socket(AF_INET, SOCK_STREAM, 0); //TCP server_socket

    setnonblocking(this->server_socket);//设置socket为非阻塞模式

    //创建一个epoll句柄
    if ((this->epollfd = epoll_create(MAX_EPOLL_FD_NUM)) < 0)
    {
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    struct epoll_event ev;
    ev.data.fd = this->server_socket;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLHUP | EPOLLRDHUP | EPOLLERR | EPOLLET;
    //将服务器socket加入到epoll的监听队列里面
    if(epoll_ctl(this->epollfd, EPOLL_CTL_ADD, this->server_socket, &ev) < 0)
    {
        printf("epoll注册监听错误: listen socket = %d\n", this->server_socket);
        exit(EXIT_FAILURE);
    }

    if (port > 1024)
    {
        struct sockaddr_in localAddr;
        memset(&localAddr, 0, sizeof(struct sockaddr_in));
        socklen_t addr_len = sizeof(struct sockaddr_in);
        localAddr.sin_family = AF_INET;
        localAddr.sin_port = htons(port);
        localAddr.sin_addr.s_addr = Manager::local_ip;

        if (!bind(this->server_socket, (struct sockaddr*)&localAddr, addr_len))
        {
            //监听
            listen(this->server_socket, MAX_EPOLL_FD_NUM);
        }
        else
        {
            cout << "TcpServerSocket绑定TCP端口" << port << "失败！" << endl;
            exit(EXIT_FAILURE);
        }
    }
}

TcpServerSocket::~TcpServerSocket()
{
//dtor
}

//不使用新线程做接收
void TcpServerSocket::startRecving()
{
    this->_runListening(this);
}

void* TcpServerSocket::_runListening(void *obj)
{
    TcpServerSocket *p = (TcpServerSocket*) obj;//当前TcpServerSocket对象指针

    p->running = true;

    //监视状态发生变化的socket
    struct epoll_event ev;//设置监听的读写事件
    struct epoll_event events[MAX_EPOLL_FD_NUM];//监听到的读写事件数组
    int num_fd_change;//状态发生变化的fd数量
    int client_socket;
    struct sockaddr_in client_addr;//客户端地址结构
    socklen_t client_addrlen = sizeof(struct sockaddr_in);//地址结构长度
    int bytes;//数据字节数
    char data[MAX_BUF_SIZE];//存储数据

    while (p->running)
    {
        //等待直到有事件发生
        //返回值为发生事件的socket数量
        num_fd_change = epoll_wait(p->epollfd, events, MAX_EPOLL_FD_NUM, -1);

        //遍历每个状态改变的socket
        for (int n = 0; n < num_fd_change; ++n)
        {
            //如果该socket是服务socket，则证明accept到新连接
            if (events[n].data.fd == p->server_socket)
            {
                memset(&client_addr, 0, sizeof(client_addr));
                client_socket = accept(p->server_socket, (struct sockaddr*)&client_addr, &client_addrlen);

                //无效套接字
                if (client_socket < 0)
                {
                    continue;
                }

                p->setnonblocking(client_socket); // 将新连接置于非阻塞模式

                //对新连接进行处理
                //p->accept_handler(client_socket,client_addr);

                //在日志中记录客户端ip地址和端口号
                string format = "服务器收到新的TCP连接请求 访问来自 %s:%u";
                char message[256];
                snprintf(message, 256, format.c_str(), inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                Manager::log.record(string(message));

                // 并且将新连接也加入EPOLL的监听队列
                ev.events = EPOLLIN | EPOLLOUT | EPOLLHUP| EPOLLRDHUP | EPOLLERR | EPOLLET;
                ev.data.fd = client_socket;

                if(epoll_ctl(p->epollfd, EPOLL_CTL_ADD, client_socket, &ev) < 0)
                {
                    string format = "epoll注册监听错误: listen socket = %d\n";
                    char message[50];
                    snprintf(message, 50, format.c_str(), client_socket);
                    Manager::log.record(message);

                    continue;
                }
            }
            //如果是已经连接的用户，并且收到数据，那么进行读入
            else if (events[n].events & EPOLLIN)
            {
                int sockfd_r; //可读socket
                //socket无效
                if ((sockfd_r = events[n].data.fd) < 0)
                {
                    p->tcp_except(sockfd_r);
                    continue;
                }

                bytes = read(sockfd_r, data, MAX_BUF_SIZE);

                //对接收到的数据进行处理
                if (bytes > 0)
                {
                    p->recv_handler(sockfd_r, data);
                }

                else if (bytes == 0)
                {
                    if (errno != EAGAIN && errno != EINTR)
                    {
                        string err = "else if (bytes == 0) errno=" + errno + string(strerror(errno));
                        Manager::log.record(err);
                        //对无效套接字进行处理或者如果是连接用户断开连接
                        p->tcp_except(sockfd_r);
                    }
                }

                //移除异常断开的连接
                else if (bytes == -1)
                {
                    if (errno != EAGAIN && errno != EINTR)
                    {
                        string err = "else if (bytes == -1) errno=" + errno + string(strerror(errno));
                        Manager::log.record(err);
                        //对无效套接字进行处理或者如果是连接用户断开连接
                        p->tcp_except(sockfd_r);
                    }
                }
            }

            //有可能捕获得到客户端连接已经断开
            else if (events[n].events & EPOLLHUP)
            {
                string err = "else if (events[n].events & EPOLLHUP) errno=" + errno + string(strerror(errno));
                Manager::log.record(err);
                int sockfd_h = events[n].data.fd;
                p->tcp_except(sockfd_h);
            }

            //有可能捕获得到客户端连接已经断开
            else if (events[n].events & EPOLLRDHUP)
            {
                string err = "else if (events[n].events & EPOLLRDHUP) errno=" + errno + string(strerror(errno));
                Manager::log.record(err);
                int sockfd_h = events[n].data.fd;
                p->tcp_except(sockfd_h);
            }

            //操作连接出错
            else if (events[n].events & EPOLLERR)
            {
                if (errno != EAGAIN && errno != EINTR)
                {
                    string err = "else if (events[n].events & EPOLLERR) errno=" + errno + string(strerror(errno));
                    Manager::log.record(err);
                    //对错误套接字进行处理
                    int sockfd_e = events[n].data.fd;
                    p->tcp_except(sockfd_e);
                }
            }
        }
    }
    return ((void*)0);
}

//使用新线程做接收
bool TcpServerSocket::startRecvingAsyn()
{
    pthread_t ntid;//新线程ID

    if (pthread_create(&ntid, NULL, TcpServerSocket::_runListening, this))
    {
        return false;
    }

    return true;
}

//关闭套接字
void TcpServerSocket::close()
{
    this->running = false;
    ::close(this->epollfd);
    ::close(this->server_socket);
}

void TcpServerSocket::recv_handler(int socket, char *data)
{
    //TCP socket 收到客户端命令时，解析并执行相应的逻辑
    //将收到的字节流第一个字节取出来，返回第一个字节码（操作码）以及剩余部分，用结构体来填充数据

    //提取操作码
    char op;
    memcpy(&op, data, sizeof(char));

    //当消息为登记客户端ID名
    if (op == REGIST_REWO)
    {
        struct regist_rewo recv_data;

        unpack_regist_rewo(&recv_data, data);

        string client_name = string(recv_data.client_name);

        //记录日志消息
        string client_type = "机器人PID";//客户端类型

        if (client_name.rfind("_mobile") != string::npos)
        {
            client_type = "手机用户";
        }

        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t client_addrlen = sizeof(struct sockaddr_in);
        getpeername(socket, (struct sockaddr*)&client_addr, &client_addrlen);
        string format = "%s %s 申请登记客户端 访问来自 %s:%u";
        char message[256];
        snprintf(message, 256, format.c_str(), client_type.c_str(), recv_data.client_name,
                 inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        Manager::log.record(string(message));

        //增加映射对
        Manager::client_socket[client_name] = socket;
        Manager::socket_client[socket] = client_name;

        RSAEncry rsa = RSAEncry();
        RSA *private_key = rsa.get_private_key();//获取密钥

        //获取密钥成功
        if (private_key)
        {
            Manager::client_pk[client_name] = private_key;//存储密钥
            char *buf = NULL;
            int key_len = rsa.get_public_key(buf, private_key);//获取公钥数据

            //提取公钥成功
            if (key_len)
            {
                struct login_pub_key send_data;
                send_data.op = LOGIN_PUB_KEY;
                send_data.key_len = htonl(key_len);

                memcpy(send_data.public_key, buf, key_len);
                free(buf);
                buf = NULL;

                char tmp[MAX_BUF_SIZE];
                pack_login_pub_key(tmp, &send_data);

                send(socket, tmp, MAX_BUF_SIZE, 0);
            }
        }
    }

    //收到加密的密码
    else if (op == LOGIN_PASSWORD)
    {
        struct login_password recv_data;

        unpack_login_password(&recv_data, data);

        string client_name = string(recv_data.client_name);
        map<string, RSA*>::iterator it = Manager::client_pk.find(client_name);

        struct login_result send_data;
        char tmp[MAX_BUF_SIZE];

        //找到密钥
        if (it != Manager::client_pk.end())
        {
            RSA *private_key = it->second;

            char *dencry_str = NULL;
            RSAEncry rsa = RSAEncry();
            rsa.dencry(dencry_str,recv_data.encry_str,private_key,128);

            //删除密钥
            free(private_key);
            private_key = NULL;
            Manager::client_pk.erase(it);

            //解密成功
            if (dencry_str)
            {
                string password = string(dencry_str);
                free(dencry_str);
                dencry_str = NULL;

                //用户是已注册用户
                if (Manager::mysqlDao.checkUser(client_name, password))
                {
                    //将用户名放入已成功登录集合中
                    Manager::logined_clients.insert(client_name);

                    //记录日志消息
                    string client_type = "机器人PID";//客户端类型

                    if (client_name.rfind("_mobile") != string::npos)
                    {
                        client_type = "手机用户";
                    }

                    struct sockaddr_in client_addr;
                    memset(&client_addr, 0, sizeof(client_addr));
                    socklen_t client_addrlen = sizeof(struct sockaddr_in);
                    getpeername(socket, (struct sockaddr*)&client_addr,
                                &client_addrlen);
                    string format = "%s %s 登录成功 访问来自 %s:%u";
                    char message[256];
                    snprintf(message, 256, format.c_str(), client_type.c_str(), client_name.c_str(),
                             inet_ntoa(client_addr.sin_addr),
                             ntohs(client_addr.sin_port));
                    Manager::log.record(string(message));

                    send_data.op = LOGIN_RESULT;
                    send_data.login_success = true;
                    pack_login_result(tmp, &send_data);

                    send(socket, tmp, MAX_BUF_SIZE, 0);
                }

                //用户非法登录
                else
                {
                    send_data.op = LOGIN_RESULT;
                    send_data.login_success = false;
                    pack_login_result(tmp, &send_data);

                    send(socket, tmp, MAX_BUF_SIZE, 0);
                }
            }

            //密码解密失败
            else
            {
                send_data.op = LOGIN_RESULT;
                send_data.login_success = false;
                pack_login_result(tmp, &send_data);

                send(socket, tmp, MAX_BUF_SIZE, 0);
            }
        }

        //找不到密钥
        else
        {
            send_data.op = LOGIN_RESULT;
            send_data.login_success = false;
            pack_login_result(tmp, &send_data);

            send(socket, tmp, MAX_BUF_SIZE, 0);
        }
    }

    //当消息为请求与目标客户端的命令端口建立连接时
    else if (op == GET_CMD_ADDR)
    {
        struct get_cmd_addr recv_data;
        unpack_get_cmd_addr(&recv_data, data);
        string source = string(recv_data.source);
        string target = string(recv_data.target);

        //判断客户端是否已成功登录
        if (Manager::logined_clients.count(source))
        {
            struct rewo_user_netinfo target_addr;
            //待发送数据
            struct notify_nat_cmd send_data;

            //获取目标客户端地址成功
            if (Manager::mysqlDao.getAddrByName(&target_addr, target))
            {
                send_data.op = NOTIFY_NAT_CMD;
                send_data.online = target_addr.online;
                send_data.port = htonl(target_addr.net_cmd_port);
                send_data.net_ip = target_addr.net_ip;

                char tmp[MAX_BUF_SIZE];
                pack_notify_nat_cmd(tmp, &send_data);

                //向源客户端发送目标客户端地址信息
                send(socket, tmp, MAX_BUF_SIZE, 0);

                //如果目标客户端在线，则向它发送源客户端地址
                if (target_addr.online)
                {
                    struct rewo_user_netinfo source_addr;

                    //获取源客户端地址
                    if (Manager::mysqlDao.getAddrByName(&source_addr, source))
                    {
                        send_data.op = NOTIFY_NAT_CMD;
                        send_data.online = source_addr.online;
                        send_data.port = htonl(source_addr.net_cmd_port);
                        send_data.net_ip = source_addr.net_ip;

                        pack_notify_nat_cmd(tmp, &send_data);

                        //向目标机器人发送源客户端地址信息
                        send(Manager::client_socket[target], tmp, MAX_BUF_SIZE, 0);
                    }
                }
            }

            //获取目标客户端地址失败
            else
            {
                send_data.op = NOTIFY_NAT_CMD;
                send_data.online = false;
                send_data.port = 0;
                send_data.net_ip = 0;

                char tmp[MAX_BUF_SIZE];
                pack_notify_nat_cmd(tmp, &send_data);

                //向源客户端发送目标客户端地址信息
                send(socket, tmp, MAX_BUF_SIZE, 0);
            }
        }

    }

    //当消息为请求与目标客户端的语音端口建立连接时
    else if (op == GET_VOICE_ADDR)
    {
        struct get_voice_addr recv_data;
        unpack_get_voice_addr(&recv_data, data);
        string source = string(recv_data.source);
        string target = string(recv_data.target);

        //判断客户端是否已成功登录
        if (Manager::logined_clients.count(source))
        {
            struct rewo_user_netinfo target_addr;
            //待发送数据
            struct notify_nat_voice send_data;

            //获取目标客户端地址成功
            if (Manager::mysqlDao.getAddrByName(&target_addr, target))
            {
                send_data.op = NOTIFY_NAT_VOICE;
                send_data.online = target_addr.online;
                send_data.port = htonl(target_addr.net_voice_port);
                send_data.net_ip = target_addr.net_ip;

                char tmp[MAX_BUF_SIZE];
                pack_notify_nat_voice(tmp, &send_data);

                //向源客户端发送目标客户端地址信息
                send(socket, tmp, MAX_BUF_SIZE, 0);

                //如果目标客户端在线，则向它发送源客户端地址
                if (target_addr.online)
                {
                    struct rewo_user_netinfo source_addr;

                    //获取源客户端地址
                    if (Manager::mysqlDao.getAddrByName(&source_addr, source))
                    {
                        send_data.op = NOTIFY_NAT_VOICE;
                        send_data.online = source_addr.online;
                        send_data.port = htonl(source_addr.net_voice_port);
                        send_data.net_ip = source_addr.net_ip;

                        pack_notify_nat_voice(tmp, &send_data);

                        //向目标机器人发送源客户端地址信息
                        send(Manager::client_socket[target], tmp, MAX_BUF_SIZE, 0);
                    }
                }
            }

            //获取目标客户端地址失败
            else
            {
                send_data.op = NOTIFY_NAT_VOICE;
                send_data.online = false;
                send_data.port = 0;
                send_data.net_ip = 0;

                char tmp[MAX_BUF_SIZE];
                pack_notify_nat_voice(tmp, &send_data);

                //向源客户端发送目标客户端地址信息
                send(socket, tmp, MAX_BUF_SIZE, 0);
            }
        }

    }

    //通知机器人更新应用配置
    else if (op == NOTIFY_UPDATE_CONFIG)
    {
        struct notity_update_config recv_data;

        unpack_notity_update_config(&recv_data, data);

        //寻找机器人socket
        map<string,int>::iterator it = Manager::client_socket.find(string(recv_data.client_name));

        if (it != Manager::client_socket.end())
        {
            //向机器人发送更新应用资源信息
            send(it->second, data, MAX_BUF_SIZE, 0);
        }
    }

    //广播通知在线机器人网站应用有更新
    else if (op == BROADCAST_UPDATE_APP)
    {
        for (map<string,int>::iterator it = Manager::client_socket.begin();
                it != Manager::client_socket.end(); ++it)
        {
            //向机器人发送有新应用可下载信息
            send(it->second, data, MAX_BUF_SIZE, 0);
        }
    }
}

//Socket断开连接时，将其对应的客户端在数据库的记录设置为不在线
void TcpServerSocket::tcp_except(int socket)
{
    //查询Socket对应的客户端
    map<int,string>::iterator it0 = Manager::socket_client.find(socket);

    //删除映射对
    if (it0 != Manager::socket_client.end())
    {
        string client_name = it0->second;
        Manager::mysqlDao.unregistAddrByName(client_name);//修改数据库，设置客户端为“不在线”
        Manager::socket_client.erase(it0);

        map<string,int>::iterator it1 = Manager::client_socket.find(client_name);

        if (it1 != Manager::client_socket.end())
        {
            Manager::client_socket.erase(it1);
        }

        //在已登录用户集合中删除该用户
        set<string>::iterator it2 = Manager::logined_clients.find(client_name);

        if (it2 != Manager::logined_clients.end())
        {
            Manager::logined_clients.erase(it2);
        }

        //记录日志
        string client_type = "机器人PID";//客户端类型

        if (client_name.rfind("_mobile") != string::npos)
        {
            client_type = "手机用户";
        }

        struct sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t client_addrlen = sizeof(struct sockaddr_in);
        getpeername(socket, (struct sockaddr*)&client_addr,
                    &client_addrlen);
        string format = "%s %s 下线 访问来自 %s:%u";
        char message[256];
        snprintf(message, 256, format.c_str(), client_type.c_str(), client_name.c_str(),
                 inet_ntoa(client_addr.sin_addr),
                 ntohs(client_addr.sin_port));
        Manager::log.record(string(message));
    }

    ::close(socket);
}







