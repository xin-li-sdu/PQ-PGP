#include "ENC_SIGN.h"
#include"base64.h"
#include<iostream>
using namespace std;
char mmm[10]="";
extern char MPK1[10000];
char sdMPK[10000];
extern GNTRU_MSK_Data * GNTRU_MSKD ;
extern GNTRU_MPK_Data * GNTRU_MPKD ;
extern int test;
ENC_SIGN::ENC_SIGN(QObject *parent)
    : QObject{parent}
{
    isLoading(false);
    networkManager = new QNetworkAccessManager(this);
}
bool ENC_SIGN::sendbase64Message(const QString& text,const QString& text_code,const QString& text_mpk)//base64测试函数
{

    uint8_t arr[]="hfaoi9w834hroqlaikjweehf7iqwh34riuhq23l4i";
    responseData((char*)arr);
    //int len = 2 * ((sizeof(arr) + 2) / 3) + 1;
    int len =sizeof(arr);
    char result[10000]; // 预留足够大的空间
    for (int i = 0; i < 10000; i++)
    {
         result[i]=0;
    }
    uint8_to_64(result, arr, len);
    responseData(result);

    int len2 = strlen(result);
    uint8_t result2[10000];
    for (int i = 0; i < 10000; i++)
    {
         result2[i]=0;
    }
//    uint64_to_8(result2,result);
    responseData((char*)(result2));
    return true;
}
bool ENC_SIGN::sendvfyMessage(const QString& text,const QString& text_code,const QString& text_mpk)//回传函数
{
    //qDebug() <<text_mpk;
    //printf("%s","text_mpk:***************************");
    QByteArray byte1 = text_code.toUtf8();
    char* buf;//签名
    buf=byte1.data();
    size_t len = byte1.size();
    QByteArray byte2 = text.toUtf8();
    char* buf2;//明文
    buf2=byte2.data();
    size_t len2 = byte2.size();
    QByteArray byte3 = text_mpk.toUtf8();
    char* buf3;//密钥
    buf3=byte3.data();
   // printf("%s",buf3);
    int check = Api_Vrfy(buf3, buf, buf2, len2);
    if(check == 0)
       return true;
    return false;
}
int ENC_SIGN::senddecvfyMessage(const QString& text,const QString& text_code)//回传函数
{
    QByteArray byte1 = text_code.toUtf8();
    char* buf;//签名
    buf=byte1.data();
    size_t len = byte1.size();
    QByteArray byte2 = text.toUtf8();
    char* buf2;//明文
    buf2=byte2.data();
    char m0[100000];
    memset(m0, 0, 100000);
    int check = Api_Unsigncrypt(GNTRU_MPKD, GNTRU_MSKD, buf2, m0, buf, len);
    //responseData(buf2);
    if(check==-1)
    {

        //responseData("-1");
      return -1;
    }
    if(check==-2)
    {

       // responseData("-2");
      return -2;
    }

     responseData(m0);//回传指令
    return 0;
}
bool ENC_SIGN::senddecMessage(const QString& text,const QString& text_code,const QString& text_mpk)//回传函数
{
    QByteArray byte1 = text_code.toUtf8();
    char* buf;
    buf=byte1.data();
    size_t len = byte1.size();
    //printf("dec len = %lu\n",len);
    char m0[100000];
    memset(m0, 0, 100000);
    int check=Api_Dec(GNTRU_MPKD, GNTRU_MSKD, m0, buf, len);
    if(check>0)
      {
        responseData(m0);//回传指令
        return true;

    }
    else return false;
}

bool ENC_SIGN::sendesMessage(const QString& text,const QString& text_code)//签密函数
{
    QByteArray byte1 = text_code.toUtf8();
    char* buf;
    buf=byte1.data();
    size_t len = byte1.size();
    QByteArray byte2 = text.toUtf8();
    char* buf2;
    buf2=byte2.data();
   // size_t len2 = byte2.size();
    char c[100000];
    memset(c, 0, 100000);
    Api_Signcrypt(buf2, GNTRU_MSKD, c, buf, len);
    //responseData(buf2);
    responseData(c);
    return true;
}


void ENC_SIGN::sendMessage(const QString& text,const QString& text_code,const QString& work)//回传函数
{
    //isLoading(true);

    QByteArray byte1 = text_code.toUtf8();
    char* buf;
    buf=byte1.data();
    size_t len = byte1.size();
    QByteArray byte2 = text.toUtf8();
    char* buf2;
    buf2=byte2.data();
    size_t len2 = byte2.size();
    if(work=="enc")//完成
    {
       // w();
        char c[100000];
        memset(c, 0, 100000);
        //printf("%zu\n",len);
        Api_Enc(buf2, c, buf, len);
        //responseData("说明这个是加密");
        responseData(c);//回传指令
        return;
        //responseData("msk模式修改首位:"+byte1);
    }
    else if(work=="sign")
    {
        char Sign[100000];
        memset(Sign, 0, 100000);
        Api_Sign(GNTRU_MSKD, Sign, buf, len);
        //responseData("说明这个是签名");
        responseData(Sign);
        return ;
    }
    else if(work=="keygen")
    {
        //test=1;
        //responseData("返回用户pk");
        responseData(MPK1);//回传指令
        return;
    }


}
QJsonObject ENC_SIGN::createMessage(const QString& role,const QString& content)
{
    QJsonObject message;
    message.insert("role",role);
    message.insert("content",content);
    return message;
}

void ENC_SIGN::clipText(const QString& text)
{
    qDebug()<<text;
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(text);
}


