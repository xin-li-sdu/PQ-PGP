//#ifndef A_H
//#define A_H
#include <QtCore>
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QGuiApplication>
#include <QClipboard>
#include <QByteArray>
#include <QString>
#include <QFile>
#include<cstdio>
#include<QChar>
#include<string.h>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include<cstring>
#include "stdafx.h"
#include "api.h"
#include "params.h"
#include"base64.h"
extern GNTRU_MSK_Data * GNTRU_MSKD ;
extern GNTRU_MPK_Data * GNTRU_MPKD ;
extern char MPK1[10000];
class ENC_SIGN : public QObject
{
    Q_OBJECT
    Q_PROPERTY_AUTO(bool,isLoading)
    Q_PROPERTY_AUTO(QString,responseData);
public:
    explicit ENC_SIGN(QObject *parent = nullptr);

    Q_INVOKABLE void sendMessage(const QString& text,const QString& text_code,const QString& work);
    Q_INVOKABLE bool sendvfyMessage(const QString& text,const QString& text_code,const QString& text_mpk);
    Q_INVOKABLE bool senddecMessage(const QString& text,const QString& text_code,const QString& text_mpk);//回传函数
    Q_INVOKABLE int senddecvfyMessage(const QString& text,const QString& text_code);//回传函数
    Q_INVOKABLE bool sendesMessage(const QString& text,const QString& text_code);//回传函数
    Q_INVOKABLE bool sendbase64Message(const QString& text,const QString& text_code,const QString& text_mpk);//basetest
    Q_INVOKABLE void sendemail(const QString& message,const QString& address,const QString& key);//发送邮件
    Q_INVOKABLE int emailget(const QString& address,const QString& key);
    Q_INVOKABLE void clipText(const QString& text);
    Q_INVOKABLE bool idfind();

private:
    QJsonObject createMessage(const QString& role,const QString& content);

private:
    QNetworkAccessManager* networkManager;
    QJsonArray messages;
    //QString baseKey = "c2stbXgxWm5MQkZ5TzhNYzNmRWl6eDZUM0JsYmtGSnNBWjNiakJjSXB6WGN3QW9KSk11";
};



//-------多线程------//
class ApiKeygenThread : public QObject {
    Q_OBJECT
public:
    explicit ApiKeygenThread(QObject *parent = nullptr) : QObject(parent) {}
signals:
    void finished();
public slots:
    void run()
    {
        srand((unsigned)time(NULL));
        GNTRU_MSKD = new GNTRU_MSK_Data;
        GNTRU_MPKD = new GNTRU_MPK_Data;
        memset(MPK1, 0, 10000);
        Api_Keygen(GNTRU_MPKD, GNTRU_MSKD, MPK1);
        emit finished();
    }
};
//------多线程---------///////


//#endif // A_H

