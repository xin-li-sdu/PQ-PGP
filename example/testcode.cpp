#include "testcode.h"
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>
void Voronoi::tDigest()
{
    unsigned char sm3_value[EVP_MAX_MD_SIZE];   //保存输出的摘要值的数组
    unsigned int sm3_len;
    int i;
    EVP_MD_CTX *sm3ctx;                         //EVP消息摘要结构体
    sm3ctx = EVP_MD_CTX_new();//调用函数初始化
    char msg1[] = "20191209";              //待计算摘要的消息1
    //char msg2[] = "xujiasheng";              //待计算摘要的消息2

    EVP_MD_CTX_init(sm3ctx);                    //初始化摘要结构体
    EVP_DigestInit_ex(sm3ctx, EVP_sm3(), NULL); //设置摘要算法和密码算法引擎，这里密码算法使用sm3，算法引擎使用OpenSSL默认引擎即软算法
    EVP_DigestUpdate(sm3ctx, msg1, strlen(msg1));//调用摘要UpDate计算msg1的摘要
    //EVP_DigestUpdate(sm3ctx, msg2, strlen(msg2));//调用摘要UpDate计算msg2的摘要
    EVP_DigestFinal_ex(sm3ctx, sm3_value, &sm3_len);//摘要结束，输出摘要值
    EVP_MD_CTX_reset(sm3ctx);                       //释放内存

    printf("原始数据%s的摘要值为:\n",msg1);
    for(i = 0; i < sm3_len; i++)
    {
        printf("0x%x ", sm3_value[i]);
    }
    printf("\n");
}
void Voronoi::test()
{
    qDebug()<<"hello form c++";
}
double Voronoi::add(double x, double y)
{
    return x+y;
}
