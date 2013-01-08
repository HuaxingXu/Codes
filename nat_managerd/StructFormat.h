#ifndef STRUCTFORMAT_H
#define STRUCTFORMAT_H

#include <memory.h>

struct regist_rewo
{
    char op; //操作码
    char client_name[60]; //客户端名
};

void unpack_regist_rewo(struct regist_rewo *des_data, char *src_data);

void pack_regist_rewo(char *des_data, struct regist_rewo *src_data);

//发送公钥
struct login_pub_key
{
    char op;
    int key_len;
    char public_key[200];
};

void unpack_login_pub_key(struct login_pub_key *des_data, char *src_data);

void pack_login_pub_key(char *des_data, struct login_pub_key *src_data);

struct login_result
{
    char op;
    bool login_success;
};

void unpack_login_result(struct login_result *des_data, char *src_data);

void pack_login_result(char *des_data, struct login_result *src_data);

struct login_password
{
    char op;
    char client_name[60];
    int encry_len;
    char encry_str[200];
};

void unpack_login_password(struct login_password *des_data, char *src_data);

void pack_login_password(char *des_data, struct login_password *src_data);

struct regist_addr
{
    char op;
    char client_name[60];
};

void unpack_regist_addr(struct regist_addr *des_data, char *src_data);

void pack_regist_addr(char *des_data, struct regist_addr *src_data);

struct get_cmd_addr
{
   char op;
   char source[60];
   char target[60];
};

void unpack_get_cmd_addr(struct get_cmd_addr *des_data, char *src_data);

void pack_get_cmd_addr(char *des_data, struct get_cmd_addr *src_data);

struct get_voice_addr
{
   char op;
   char source[60];
   char target[60];
};

void unpack_get_voice_addr(struct get_voice_addr *des_data, char *src_data);

void pack_get_voice_addr(char *des_data, struct get_voice_addr *src_data);

struct notify_nat_cmd
{
    char op;
    bool online;
    unsigned int port;
    unsigned int net_ip;
};

void unpack_notify_nat_cmd(struct notify_nat_cmd *des_data, char *src_data);

void pack_notify_nat_cmd(char *des_data, struct notify_nat_cmd *src_data);

struct notify_nat_voice
{
    char op;
    bool online;
    unsigned int port;
    unsigned int net_ip;
};

void unpack_notify_nat_voice(struct notify_nat_voice *des_data, char *src_data);

void pack_notify_nat_voice(char *des_data, struct notify_nat_voice *src_data);

struct notity_update_resource
{
    char op;
    char client_name[60];
    int app_id;
    char aciton[10];//应用配置发生的操作，add,delete,modify
};

void unpack_notity_update_resource(struct notity_update_resource *des_data, char *src_data);

void pack_notity_update_resource(char *des_data, struct notity_update_resource *src_data);

struct notity_update_config
{
    char op;
    char client_name[60];
    int app_id;
    char action[10];
};

void unpack_notity_update_config(struct notity_update_config *des_data, char *src_data);

void pack_notity_update_config(char *des_data, struct notity_update_config *src_data);

struct broadcast_update_app
{
    char op;
    int app_id;
    char action[10];
};

void unpack_broadcast_new_app(struct broadcast_update_app *des_data, char *src_data);

void pack_broadcast_new_app(char *des_data, struct broadcast_update_app *src_data);

struct rewo_user_netinfo
{
    unsigned int net_ip;
    unsigned int net_cmd_port;
    unsigned int net_voice_port;
    bool online;
};

#endif // STRUCTFORMAT_H
