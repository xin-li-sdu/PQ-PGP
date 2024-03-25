#include "ENC_SIGN.h"
#include"base64.h"
#include"fileread.h"
#include<iostream>
#include<WinSock.h>
#include <string>
#include<windows.h>
#include<stdio.h>
#include "smtp.h"
extern std::string m_id;
using namespace std;
char mmm[10]="";
extern char MPK1[10000];
char sdMPK[10000];
extern GNTRU_MSK_Data * GNTRU_MSKD ;
extern GNTRU_MPK_Data * GNTRU_MPKD ;
extern int test;
extern char B[5465];
#pragma comment(lib,"ws2_32.lib")
ENC_SIGN::ENC_SIGN(QObject *parent)
    : QObject{parent}
{
    isLoading(false);
    networkManager = new QNetworkAccessManager(this);
}
void sendmessge(const std::string& str, char* c,char* key)//发送邮件
{
    const char* cstr = str.c_str();//id
    const char* mstr = m_id.c_str();//mid

    //Smtp smtp("lixinsdrz@163.com", key);//"UFXVPPQXXXMFLHJL"
     Smtp smtp(mstr, key);//"UFXVPPQXXXMFLHJL"
    //strcat(c,"< encrypt by PQPGP>");
    smtp.sendem(cstr, "email encrypt by PQPGP",c);
    //smtp.sendem("2635410120@qq.com", "email encrypt by PQPGP",c);
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
bool ENC_SIGN::sendvfyMessage(const QString& text,const QString& text_code,const QString& text_mpk)//认证回传函数
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
int ENC_SIGN::senddecvfyMessage(const QString& text,const QString& text_code)//认证解密回传函数，text公钥，code密文
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
      return -1;
    if(check==-2)
      return -2;
    responseData(m0);//回传指令
    return 0;
}
bool ENC_SIGN::senddecMessage(const QString& text,const QString& text_code,const QString& text_mpk)//解密回传函数
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

bool ENC_SIGN::sendesMessage(const QString& text,const QString& text_code)//签密函数，text/buf2对方公钥和text_code/buf信息,支持ID
{
    QByteArray byte1 = text_code.toUtf8();
    char* buf;
    buf=byte1.data();
    size_t len = byte1.size();
    QByteArray byte2 = text.toUtf8();
    char* buf2;
    buf2=byte2.data();
    size_t len2 = byte2.size();
    if(len2>100)//是公钥
    {
        char c[100000];
        memset(c, 0, 100000);
        Api_Signcrypt(buf2, GNTRU_MSKD, c, buf, len);
        responseData(c);//签名结果
    }
    else
    {
        char c[100000];
        memset(c, 0, 100000);
        memset(B, 0, 5465);
        std::string str = text.toStdString();//输入的是ID的话
        if(fileread(str)==1)
          {
            Api_Signcrypt(B, GNTRU_MSKD, c, buf, len);
            //Api_Enc(B, c, buf, len);//输入的是ID钥的话
            responseData(c);//回传指令
        }
        else
        {
            responseData("ID或密钥不存在");
        }
    }
    return true;
}



void ENC_SIGN::sendMessage(const QString& text,const QString& text_code,const QString& work)//多重回传函数
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
    if(work=="enc")//text是ID或公钥,text_code是信息
    {
        char c[100000];
        memset(c, 0, 100000);
        if(len2>100)//判断是不是密钥，如果很小说明是ID
        {
            Api_Enc(buf2, c, buf, len);//输入的是密钥的话
            responseData(c);//回传指令
        }
        else
        {
            memset(B, 0, 5465);
            std::string str = text.toStdString();//输入的是ID的话
            if(fileread(str)==1)
              {
                Api_Enc(B, c, buf, len);//输入的是ID钥的话
                responseData(c);//回传指令
            }
            else
            {
                responseData("ID或密钥不存在");
            }
        }

        return;
        //responseData("msk模式修改首位:"+byte1);
    }
    else if(work=="sign")
    {

        char Sign[100000];
        memset(Sign, 0, 100000);
        Api_Sign(GNTRU_MSKD, Sign, buf, len);
        responseData(Sign);
        return ;
    }
    else if(work=="keygen")//text_code是邮箱
    {
        //test=1;
        //responseData("返回用户pk");

        std::string str = text_code.toStdString();//把生成的公钥写入文件,str是ID/文件名

        //qDebug() << "My name is "<<m_id;

        responseData(MPK1);//回传指令
        if(str!="text")
        {
            MPKfilewrite(str);
            m_id=str;//存储文件名到主进程
            responseData("已注册，当前ID："+text_code);
        }

        return;
    }

}

void ENC_SIGN::sendemail(const QString& message,const QString& address,const QString& key)//发送邮件
{

    QByteArray byte1 = message.toUtf8();
    char* buf;
    buf=byte1.data();
    size_t len = byte1.size();

    QByteArray byte2 = address.toUtf8();
    char* buf2;
    buf2=byte2.data();
    size_t len2 = byte2.size();

    QByteArray byte3 = key.toUtf8();
    char* buf3;
    buf3=byte3.data();
    size_t len3 = byte3.size();

        char c[200000];
        memset(c, 0, 200000);
        if(len2>100)//判断是不是密钥，如果很小说明是ID
        {
            responseData("选择发送功能请使用ID(邮件地址)，而不是公钥");//回传指令
        }
        else
        {
            memset(B, 0, 5465);
            std::string str = address.toStdString();//输入的是ID的话
            if(fileread(str)==1)//此时利用ID将MPK放到B中
              {
                Api_Enc(B, c, buf, len);//输入的是密钥的话
                responseData(c);//回传指令
                sendmessge(str,c,buf3);
            }
            else
            {
                responseData("ID或密钥不存在");
            }
        }

        return;

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

bool ENC_SIGN::idfind()//搜索
{
    responseData("【 已注册ID 】");
    std::string directory = "C:/Users/Public/Documents";  // 替换成你要搜索的目录路径
    std::string result;
    searchFiles(directory, result);
    QString qstr = QString::fromStdString(result);
    responseData(qstr);//回传指令
}
// 打开TCP Socket连接
int OpenSocket(struct sockaddr *addr)
{
    int sockfd =0;
    sockfd=socket(PF_INET,SOCK_STREAM,0);
    if(sockfd <0)
    {
        cout<<"Open sockfd(TCP) error!"<<endl;
        exit(-1);
    }
    if(connect(sockfd,addr,sizeof(struct sockaddr))<0)
    {
        cout<<"Connect sockfd(TCP) error!"<<endl;
        exit(-1);
    }
    return sockfd;
}
int ENC_SIGN::emailget(const QString& address,const QString& key)
{
    //responseData("链接服务器时间可能较长，请耐心等待");
    int sockfd ={0};
    char buf[50000]={0};
    char rbuf[50000]={0};//POP协议传送邮件的应答
    char login[128]={0};
    char pass[128]={0};
    char* ansemail = new char[30000];//char ansemail[30000];
    //memset(ansemail,0,30000);
    std::string input;
    do
    {
        WSADATA WSAData;
        struct sockaddr_in their_addr ={0};
        WSAStartup(MAKEWORD(2,2),&WSAData);
        memset(&their_addr,0,sizeof(their_addr));
        their_addr.sin_family =AF_INET;
        their_addr.sin_port =htons(110);
        hostent*hptr =gethostbyname("pop3.163.com"); // 用的是163服务器
        memcpy(&their_addr.sin_addr.S_un.S_addr,hptr->h_addr_list[0],hptr->h_length);
        printf("IP of pop3.163.com is : %d:%d:%d:%d\n",
        their_addr.sin_addr.S_un.S_un_b.s_b1,
        their_addr.sin_addr.S_un.S_un_b.s_b2,
        their_addr.sin_addr.S_un.S_un_b.s_b3,
        their_addr.sin_addr.S_un.S_un_b.s_b4);
        //OK,163邮箱的IP地址已获取到，下面就要开始进行连接了
        sockfd =OpenSocket((struct sockaddr *)&their_addr);
        //cout<<"look:"<<sockfd<<endl;
        memset(rbuf,0,50000);
        while(recv(sockfd,rbuf,50000,0)==0)
        {
            cout<<"reconnect..."<<endl;
            Sleep(5);
            sockfd =OpenSocket((struct sockaddr *)&their_addr);
            memset(rbuf,0,50000);
        }
        cout<<rbuf<<endl;//返回220表示recv成功（访问163邮箱成功）

        // USER
        memset(buf,0,50000);
        sprintf(buf,"%s\r\n","user lixinsdrz@163.com");//输入你的邮箱账号
        //sprintf(buf,"%s\r\n","user "+address);//输入你的邮箱账号
        send(sockfd,buf,strlen(buf),0);
        //cout<<buf<<endl;
        memset(rbuf,0,50000);
        recv(sockfd,rbuf,50000,0);
        cout<<rbuf<<endl;

        // PASSWORD
        sprintf(buf,"%s\r\n","pass UFXVPPQXXXMFLHJL");//你的邮箱密码
        send(sockfd,buf,strlen(buf),0);
        //cout<<buf<<endl;
        memset(rbuf,0,50000);
        recv(sockfd,rbuf,50000,0);
        cout<<"message num:"<<rbuf<<endl;
        if(rbuf[4]<'9'&&rbuf[4]>'0'&&(rbuf[5]<'0'||rbuf[5]>'9'))
           {
             char listnum[6]="retr ";
             listnum[5]=rbuf[4];
             cout<<listnum<<endl;//得到有多少数字；
             memset(buf,0,50000);
             sprintf(buf,"%s\r\n",listnum);
            }
        else if(rbuf[4]<'9'&&rbuf[4]>'0'&&rbuf[5]>'0'&&rbuf[5]<'9')
        {
          char listnum[7]="retr ";
          listnum[5]=rbuf[4];
          listnum[6]=rbuf[5];
          cout<<listnum<<endl;//得到有多少数字；
          memset(buf,0,50000);
          sprintf(buf,"%s\r\n",listnum);
         }

       // memset(buf,0,50000);
       // sprintf(buf,"%s\r\n","retr 5");
        send(sockfd,buf,strlen(buf),0);
        memset(rbuf,0,50000);
        recv(sockfd,rbuf,50000,0);
        //cout<<rbuf<<endl;
        //cout<<"通过retr"<<endl;
        Sleep(5*1000);
        //cout<<"is quiting ...\n\n\n";
        // QUIT
        sprintf(buf,"QUIT\r\n");
        send(sockfd,buf,strlen(buf),0);
        memset(rbuf,0,50000);
        recv(sockfd,rbuf,50000,0);
        Sleep(5*1000);

        qDebug()<<"Quit Receive: "<<rbuf;//输出邮件
        //char ansemail[300000] = {0};
        size_t rbuf_length = strlen(rbuf);

        input.assign(rbuf, rbuf_length);
        if(input.find("Received:") == 0)
         {
            // responseData("连接成功，解密中");
            std::cout << "yes! 'Received:'" << std::endl;
            // 查找最后一个 \n 并提取之后的部分
            int n_num=0,head=0,tail=0;
            for(int ll=50000;ll>0;ll--)
            {
                if(rbuf[ll]=='\n')
                {
                    if(n_num==1) tail=ll;
                    if(n_num==2) head=ll;
                    n_num++;
                }
            }
            cout<<head<<" "<<tail<<endl;
            cout<<rbuf[head+1]<<" and "<<rbuf[tail-2]<<endl;//head找到了，taily也找到了
            for(int kk=0;kk<tail-head-1;kk++)
            {
                ansemail[kk]=rbuf[kk+head+1];
            }
            ansemail[tail-head-1]='\0';
//            for(int kk=0;kk<tail-head-1;kk++)
//            {
//                qDebug()<<kk<<" "<<ansemail[kk];
//            }
           // qDebug()<<ansemail[0]<<"and"<<ansemail[tail-head-2];
            //qDebug()<<ansemail;
            char m0[50000];
            memset(m0, 0, 50000);
            int len = strlen(ansemail)-1;
             qDebug()<<"len:"<<len;
            //responseData(ansemail);//回传指令
            int check=Api_Dec(GNTRU_MPKD, GNTRU_MSKD, m0, ansemail, len);
            if(check>0)
              {
                responseData("sender:lixinsdrz@163.com , IP of pop3.163.com is : 103:74:29:45");//回传指令
                responseData(m0);//回传指令
                cout<<"yes we did";
            }
            else cout<<"no,we can't";

            return 0;
          }
           else
               {
               std::cout << "no!'Received:'" << std::endl;
               // responseData("服务器连接异常，正在重启连接");
            }

        closesocket(sockfd);//清理工作
        WSACleanup();//清理工作
       // Sleep(5*1000);

    }while(input.find("Received:")!= 0);


    return 0;
}
