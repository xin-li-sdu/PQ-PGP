#ifndef ABC_H
#define ABC_H
#include <stdio.h>
#include <string.h>
#include<string>
#include <cstring>
#define NOMINMAX
#include<QString>
#include <stdint.h>
//#include <windows.h>
#include <NTL/ZZ.h>
//#include <NTL/ZZX.h>
#include <NTL/ZZXFactoring.h>
#include "gmp.h"
//#include <WinSock2.h>
//#include "Iphlpapi.h"
//#include "openssl/md5.h"
using namespace std;
using namespace NTL;
double add(int num);
QString pkword();
#include <openssl/evp.h>
int aes111();
int ccm_decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *aad, int aad_len,
                unsigned char *tag,
                unsigned char *key,
                unsigned char *iv,
                unsigned char *plaintext);
int ccm_encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *aad, int aad_len,
                unsigned char *key,
                unsigned char *iv,
                unsigned char *ciphertext,
                unsigned char *tag);
void handleErrors();
#endif // ABC_H
