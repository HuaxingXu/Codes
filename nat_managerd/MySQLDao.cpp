#include "MySQLDao.h"

//初始化数据库
string MySQLDao::server = "localhost";
string MySQLDao::user = "wuzx";
string MySQLDao::psw = "123456";
string MySQLDao::db = "think_rewo";

MySQLDao::MySQLDao()
{
    mysql_init(&(this->conn));//初始化数据库连接
}

MySQLDao::~MySQLDao()
{
    //dtor
}

//连接数据库
bool MySQLDao::connect()
{
    //启用MySQＬ服务器的自动再连接选项
    bool arg = true;

    //设置自动连接选项出错
    if (mysql_options(&(this->conn),MYSQL_OPT_RECONNECT,&arg))
    {
        cout << "设置自动连接选项出错！" << endl;
        return false;
    }

    //设置默认的字符集为uft-8
    if (mysql_options(&(this->conn),MYSQL_SET_CHARSET_NAME,"utf8"))
    {
        cout << "设置默认的字符集为uft-8出错！" << endl;
        return false;
    }

    //连接MySQL数据库
    if (!mysql_real_connect(&(this->conn),MySQLDao::server.c_str(),MySQLDao::user.c_str(),
                            MySQLDao::psw.c_str(),MySQLDao::db.c_str(),0,NULL,0))
    {
        return false;
    }

    return true;
}

//通过用户名获取用户ID
int MySQLDao::_getIdByName(string user_name)
{
    string format = "SELECT id FROM rewo_user WHERE username = \"%s\"";//查询格式

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), user_name.c_str());//格式化查询语句

    if (mysql_query(&(this->conn),query))
    {
        return -1;
    }

    //检索结果集到客户端
    MYSQL_RES *result = mysql_store_result(&(this->conn));

    //查询查结为0
    if (!result)
    {
        return -1;
    }

    //读取一行记录
    MYSQL_ROW row = mysql_fetch_row(result);
    int ret = (row ? ((int) row[0]) : -1);//返回用户ID
    mysql_free_result(result);//释放分配给结果集的内存

    return ret;
}

//通过客户端名获取产品网络地址
bool MySQLDao::getAddrByName(struct rewo_user_netinfo *addr, string client_name)
{
    //查询语句格式
    string format = "SELECT net_ip, net_cmd_port, net_voice_port, online FROM rewo_user_netinfo "
                    "WHERE client_name = \"%s\"";

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), client_name.c_str());//格式化查询语句

    if (mysql_query(&(this->conn),query))
    {
        return false;
    }

    MYSQL_RES *result = mysql_store_result(&(this->conn));

    if (!result)
    {
        return false;
    }

    MYSQL_ROW row = mysql_fetch_row(result);

    //获取记录行失败
    if (!row)
    {
        mysql_free_result(result);
        return false;
    }

    addr->net_ip = atoi(row[0]);
    addr->net_cmd_port = atoi(row[1]);
    addr->net_voice_port = atoi(row[2]);
    addr->online = (bool) atoi(row[3]);

    mysql_free_result(result);

    return true;
}

//根据产品ID插入网络地址
bool MySQLDao::insertAddr(string client_name, vector<unsigned int> addr, int port_type)
{
    vector<string> port(2,"");
    port[0] = "net_cmd_port";
    port[1] = "net_voice_port";

    //查询语句格式
    string format = "INSERT INTO rewo_user_netinfo(client_name, net_ip, " + port[port_type] +
                    ", online) VALUES(\"%s\", %u, %u, 1)";

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), client_name.c_str(), (unsigned int) addr[0],addr[1]);

    if (mysql_query(&(this->conn),query))
    {
        return false;
    }

    return true;
}

//更新地址
bool MySQLDao::updateAddr(string client_name, vector<unsigned int> addr, int port_type)
{
    vector<string> port(2,"");
    port[0] = "net_cmd_port";
    port[1] = "net_voice_port";

    string format = "UPDATE rewo_user_netinfo SET net_ip = %u," + port[port_type] +
                    " = %u, online = 1 WHERE client_name = \"%s\"";

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), addr[0], addr[1], client_name.c_str());

    if (mysql_query(&(this->conn),query))
    {
        return false;
    }

    return true;
}

//根据用户名注册地址
bool MySQLDao::registAddrByName(string client_name, vector<unsigned int> addr, int port_type)
{
    string format = "SELECT * FROM rewo_user_netinfo WHERE client_name = \"%s\"";

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), client_name.c_str());

    if (mysql_query(&(this->conn),query))
    {
        return false;
    }

    MYSQL_RES *result = mysql_store_result(&(this->conn));

    //该用户未注册地址
    if (result)
    {
        //没有相应的记录
        if (mysql_num_rows(result) == 0)
        {
            return this->insertAddr(client_name, addr, port_type);
        }
    }

    mysql_free_result(result);

    return this->updateAddr(client_name, addr, port_type);
}

//根据用户名注销地址
bool MySQLDao::unregistAddrByName(string client_name)
{
    string format = "UPDATE rewo_user_netinfo SET online = 0 WHERE client_name = \"%s\"";

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), client_name.c_str());

    if (mysql_query(&(this->conn),query))
    {
        return false;
    }

    return true;
}

//判断用户是否是已注册用户
bool MySQLDao::checkUser(string client_name, string password)
{
    //判断客户端是否是手机
    string::size_type pos = client_name.rfind("_mobile");

    if (pos != string::npos)
    {
        //提取真正的用户名
        client_name = client_name.substr(0, pos);
    }

    string format = "SELECT u_p.* FROM rewo_user AS u LEFT JOIN rewo_user_product AS u_p ON u.id"
                    " = u_p.user_id WHERE (u.id = \"%s\" AND u.password = \"%s\")"
                    " OR (u_p.product_id = \"%s\" AND u.password = \"%s\")";

    char query[1024];//最终查询语句

    snprintf(query, 1024, format.c_str(), client_name.c_str(), password.c_str(),
             client_name.c_str(), password.c_str());

    if (mysql_query(&(this->conn), query))
    {
        return false;
    }

    MYSQL_RES *result = mysql_store_result(&(this->conn));

    if (!result)
    {
        return false;
    }

    //没有相应的记录
    if (mysql_num_rows(result) == 0)
    {
        mysql_free_result(result);
        return false;
    }

    mysql_free_result(result);

    return true;
}

//关闭数据库连接
void MySQLDao::close()
{
    mysql_close(&(this->conn));
}






