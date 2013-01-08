#ifndef RSAENCRY_H
#define RSAENCRY_H

#include <string.h>
#include<openssl/rsa.h>
#include <openssl/bn.h>
#include<openssl/pem.h>
#include<openssl/err.h>

using namespace std;

class RSAEncry
{
    public:
        RSAEncry();
        ~RSAEncry();
        int encry(char *(&target), char *source, RSA *public_key);
        bool dencry(char *(&target), char *source, RSA *private_key, int encry_size);
        RSA* get_private_key();
        int get_public_key(char *(&buf), RSA *private_key);
        RSA* generate_public_key(char *buf);
    protected:
    private:

};

#endif // RSAENCRY_H
