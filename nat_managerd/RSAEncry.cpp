#include "RSAEncry.h"
#include <iostream>
using namespace std;
RSAEncry::RSAEncry()
{

}

RSAEncry::~RSAEncry()
{

}

//加密
int RSAEncry::encry(char *(&target), char *source, RSA *public_key)
{
    if (!public_key)
    {
        target = NULL;
        return 0;
    }

    //rsa的长度
    int rsa_len = RSA_size(public_key);

    //分配存储密文的内存空间
    char *p_encry = (char*) calloc(rsa_len, sizeof(char));

    if (!p_encry)
    {
        return 0;
    }

    int encry_size = 0;

    //加密数据
    if ((encry_size = (RSA_public_encrypt(strlen(source), (unsigned char*) source,
                                          (unsigned char*)p_encry, public_key, RSA_PKCS1_OAEP_PADDING))) < 0)
    {
        target = NULL;
        free(p_encry);
        p_encry = NULL;
        return 0;
    }

    target = p_encry;

    return encry_size;
}

//解密
bool RSAEncry::dencry(char *(&target), char *source, RSA *private_key, int encry_size)
{
    if (!private_key)
    {
        target = NULL;
        return false;
    }

    //rsa的长度
    int rsa_len=RSA_size(private_key);

    //分配存储明文的内存空间
    char *p_dencry = (char*) calloc(rsa_len, sizeof(char));

    if (!p_dencry)
    {
        return false;
    }

    //解密数据
    if (RSA_private_decrypt(encry_size, (unsigned char*) source,
                            (unsigned char*)p_dencry, private_key,
                            RSA_PKCS1_OAEP_PADDING) < 0)
    {
        target = NULL;
        free(p_dencry);
        p_dencry = NULL;
        return false;
    }

    target = p_dencry;

    return true;
}

//获取密钥
RSA* RSAEncry::get_private_key()
{
    return RSA_generate_key(1024, RSA_F4, NULL, NULL);
}

//获取公钥二进制数据
int RSAEncry::get_public_key(char *(&buf), RSA *private_key)
{
    if (!private_key)
    {
        return 0;
    }

    char* n_b = (char*) calloc(RSA_size(private_key), sizeof(char));
    char* e_b = (char*) calloc(RSA_size(private_key), sizeof(char));

    if (!n_b || !e_b)
    {
        return 0;
    }

    int n_size = BN_bn2bin(private_key->n, (unsigned char*)n_b);
    int e_size = BN_bn2bin(private_key->e, (unsigned char*)e_b);

    //生成一个n+e还加上两位int型的数据的数据空间
    int len = n_size + e_size + 2 * sizeof(int);

    buf = (char*) calloc(len, sizeof(char));

    if (!buf)
    {
        free(n_b);
        n_b = NULL;
        free(e_b);
        e_b = NULL;

        return 0;
    }

    int offset = 0;
    memcpy(buf, &n_size, 4);

    offset += 4;
    memcpy(buf+offset,n_b, n_size);

    offset += n_size;
    memcpy(buf+offset, &e_size, 4);

    offset += 4;
    memcpy(buf+offset,e_b, e_size);

    free(n_b);
    n_b = NULL;
    free(e_b);
    e_b = NULL;

    return len;
}

RSA* RSAEncry::generate_public_key(char *buf)
{
    int n_size, e_size;
    int offset = 0;

    memcpy(&n_size, buf, 4);

    offset += 4;
    char *n_b = (char*) calloc(n_size, sizeof(char));

    if (!n_b)
    {
        return NULL;
    }

    memcpy(n_b, buf+offset, n_size);

    offset += n_size;
    memcpy(&e_size, buf+offset, 4);

    offset += 4;
    char *e_b = (char*) calloc(e_size, sizeof(char));

    if (!e_b)
    {
        return NULL;
    }

    memcpy(e_b, buf+offset, e_size);

    RSA *public_key = RSA_new();

    if (!public_key)
    {
        return NULL;
    }

    public_key->n = BN_bin2bn((unsigned char*)n_b, n_size, NULL);
    public_key->e = BN_bin2bn((unsigned char*)e_b, e_size, NULL);

    free(n_b);
    n_b = NULL;
    free(e_b);
    e_b = NULL;

    return public_key;
}
