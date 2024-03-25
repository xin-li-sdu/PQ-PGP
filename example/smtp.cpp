#pragma execution_character_set("utf-8")
#include "smtp.h"
#include<qDebug>

Smtp::Smtp(QByteArray username,QByteArray password)
{
    if(username.contains("@163"))
    {
        this->username = username;
        this->password = password;
    }
    else
        qDebug()<<"NOT 163";
}

void Smtp::sendem(QByteArray recvaddr,QString subject,QString content)
{
    this->recvaddr = recvaddr;
    this->subject = subject;
    this->content = content;
    QByteArray usernametmp = this->username;
    QByteArray recvaddrtmp = this->recvaddr;

    clientsocket=new QTcpSocket();
    this->clientsocket->connectToHost("smtp.163.com",25,QTcpSocket::ReadWrite);//25端口
    this->clientsocket->waitForConnected(1000);
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;


    this->clientsocket->write("HELO smtp.163.com\r\n");
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;

    this->clientsocket->write("AUTH LOGIN\r\n");
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;

    this->clientsocket->write(username.toBase64().append("\r\n"));
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;

    this->clientsocket->write(password.toBase64().append("\r\n"));
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
    qDebug()<<recvdata;

    this->clientsocket->write(mailfrom.append(usernametmp.append(">\r\n")));
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;

    this->clientsocket->write(rcptto.append(recvaddrtmp.append(">\r\n")));
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;

    this->clientsocket->write("data\r\n");
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
//    qDebug()<<recvdata;


    usernametmp = this->username;
    recvaddrtmp = this->recvaddr;

    this->clientsocket->write(prefrom.append(usernametmp.append("\r\n")));
    this->clientsocket->write(preto.append(recvaddrtmp.append("\r\n")));
    this->clientsocket->write(presubject.append(subject.toUtf8().append("\r\n")));
    this->clientsocket->write("\r\n");
    this->clientsocket->write(content.toUtf8().append("\r\n"));
    this->clientsocket->write(".\r\n");
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
    qDebug()<<recvdata;

    this->clientsocket->write("quit\r\n");
    this->clientsocket->waitForReadyRead(1000);
    recvdata = clientsocket->readAll();
    qDebug()<<recvdata;
}

Smtp::~Smtp()
{
    if(this->clientsocket)
        delete this->clientsocket;
}

