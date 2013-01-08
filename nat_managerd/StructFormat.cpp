#include "StructFormat.h"

//解包和打包相关的结构体数据
void unpack_regist_rewo(struct regist_rewo *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->client_name,src_data + offset, size);
}

void pack_regist_rewo(char *des_data, struct regist_rewo *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->client_name, size);
}

void unpack_login_pub_key(struct login_pub_key *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->key_len);
    memcpy(&des_data->key_len,src_data + offset, size);

    offset += size;
    size = 200;
    memcpy(des_data->public_key,src_data + offset, size);
}

void pack_login_pub_key(char *des_data, struct login_pub_key *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = sizeof(src_data->key_len);
    memcpy(des_data + offset, &src_data->key_len, size);

    offset += size;
    size = 200;
    memcpy(des_data + offset, src_data->public_key, size);
}

void unpack_login_result(struct login_result *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->login_success);
    memcpy(&des_data->login_success,src_data + offset, size);
}

void pack_login_result(char *des_data, struct login_result *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = sizeof(src_data->login_success);
    memcpy(des_data + offset, &src_data->login_success, size);
}

void unpack_login_password(struct login_password *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->client_name,src_data + offset, size);

    offset += size;
    size = sizeof(des_data->encry_len);
    memcpy(&des_data->encry_len,src_data + offset, size);

    offset += size;
    size = 128;
    memcpy(des_data->encry_str,src_data + offset, size);
}

void pack_login_password(char *des_data, struct login_password *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->client_name, size);

    offset += size;
    size = sizeof(src_data->encry_len);
    memcpy(des_data + offset, &src_data->encry_len, size);

    offset += size;
    size = 200;
    memcpy(des_data + offset, src_data->encry_str, size);
}

void unpack_regist_addr(struct regist_addr *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->client_name,src_data + offset, size);
}

void pack_regist_addr(char *des_data, struct regist_addr *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->client_name, size);
}

void unpack_get_cmd_addr(struct get_cmd_addr *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->source, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->target, src_data + offset, size);
}

void pack_get_cmd_addr(char *des_data, struct get_cmd_addr *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->source, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->target, size);
}

void unpack_get_voice_addr(struct get_voice_addr *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->source, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->target, src_data + offset, size);
}

void pack_get_voice_addr(char *des_data, struct get_voice_addr *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->source, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->target, size);
}

void unpack_notify_nat_cmd(struct notify_nat_cmd *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->online);
    memcpy(&des_data->online, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->port);
    memcpy(&des_data->port, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->net_ip);
    memcpy(&des_data->net_ip, src_data + offset, size);
}

void pack_notify_nat_cmd(char *des_data, struct notify_nat_cmd *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = sizeof(src_data->online);
    memcpy(des_data + offset, &src_data->online, size);

    offset += size;
    size = sizeof(src_data->port);
    memcpy(des_data + offset, &src_data->port, size);

    offset += size;
    size = sizeof(src_data->net_ip);
    memcpy(des_data + offset, &src_data->net_ip, size);
}

void unpack_notify_nat_voice(struct notify_nat_voice *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->online);
    memcpy(&des_data->online, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->port);
    memcpy(&des_data->port, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->net_ip);
    memcpy(&des_data->net_ip, src_data + offset, size);
}

void pack_notify_nat_voice(char *des_data, struct notify_nat_voice *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = sizeof(src_data->online);
    memcpy(des_data + offset, &src_data->online, size);

    offset += size;
    size = sizeof(src_data->port);
    memcpy(des_data + offset, &src_data->port, size);

    offset += size;
    size = sizeof(src_data->net_ip);
    memcpy(des_data + offset, &src_data->net_ip, size);
}

void unpack_notity_update_resource(struct notity_update_resource *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->client_name, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->app_id);
    memcpy(&des_data->app_id, src_data + offset, size);

    offset += size;
    size = 10;
    memcpy(des_data->aciton, src_data + offset, size);
}

void pack_notity_update_resource(char *des_data, struct notity_update_resource *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->client_name, size);

    offset += size;
    size = sizeof(src_data->app_id);
    memcpy(des_data + offset, &src_data->app_id, size);

    offset += size;
    size = 10;
    memcpy(des_data + offset, src_data->aciton, size);
}

void unpack_notity_update_config(struct notity_update_config *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = 60;
    memcpy(des_data->client_name, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->app_id);
    memcpy(&des_data->app_id, src_data + offset, size);

    offset += size;
    size = 10;
    memcpy(des_data->action, src_data + offset, size);
}

void pack_notity_update_config(char *des_data, struct notity_update_config *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = 60;
    memcpy(des_data + offset, src_data->client_name, size);

    offset += size;
    size = sizeof(src_data->app_id);
    memcpy(des_data + offset, &src_data->app_id, size);

    offset += size;
    size = 10;
    memcpy(des_data + offset, src_data->action, size);
}

void unpack_broadcast_new_app(struct broadcast_update_app *des_data, char *src_data)
{
    int size = sizeof(des_data->op);
    int offset = 0;
    memcpy(&des_data->op, src_data + offset, size);

    offset += size;
    size = sizeof(des_data->app_id);
    memcpy(&des_data->app_id, src_data + offset, size);

    offset += size;
    size = 10;
    memcpy(des_data->action, src_data + offset, size);
}

void pack_broadcast_new_app(char *des_data, struct broadcast_update_app *src_data)
{
    int size = sizeof(src_data->op);
    int offset = 0;
    memcpy(des_data + offset, &src_data->op, size);

    offset += size;
    size = sizeof(src_data->app_id);
    memcpy(des_data + offset, &src_data->app_id, size);

    offset += size;
    size = 10;
    memcpy(des_data + offset, src_data->action, size);
}
