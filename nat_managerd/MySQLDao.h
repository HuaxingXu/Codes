#ifndef MYSQLDAO_H
#define MYSQLDAO_H

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <mysql/mysql.h>
#include <vector>
#include <stdlib.h>
#include "StructFormat.h"

#define CMD_PORT 0
#define VOICE_PORT 1

using namespace std;


class MySQLDao
{
    public:
        static string server;//数据库服务器
        static string user;//用户名
        static string psw;//密码
        static string db;//数据库

        MySQLDao();
        ~MySQLDao();
        bool connect();//连接数据库
        int _getIdByName(string user_name);//通过用户名获取用户ID
        bool getAddrByName(struct rewo_user_netinfo *addr, string client_name);//通过用户名获取用户网络地址
        bool insertAddr(string client_name, vector<unsigned int> addr, int port_type);//插入新地址
        bool updateAddr(string client_name, vector<unsigned int> addr, int port_type);//更新地址
        bool registAddrByName(string client_name, vector<unsigned int> addr, int port_type);//根据用户名注册地址
        bool unregistAddrByName(string client_name);//根据用户名注销地址
        bool checkUser(string client_name, string password);//检查用户是否已经注册
        void close();//关闭数据库连接

    protected:

    private:
        MYSQL conn;//数据库连接

};

#endif // MYSQLDAO_H
