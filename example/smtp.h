#ifndef SMTP_H
#define SMTP_H

#include <QObject>
#include <QTcpSocket>

class Smtp : public QObject
{
Q_OBJECT
public:
explicit Smtp(QByteArray username, QByteArray password);
void sendem(QByteArray recvaddr, QString subject, QString content);
~Smtp();

private:
QByteArray username;
QByteArray password;

QTcpSocket* clientsocket;
QByteArray mailfrom = "MAIL FROM:<";
QByteArray rcptto = "RCPT TO:<";
QByteArray prefrom = "From: ";
QByteArray preto = "To: ";
QByteArray presubject = "Subject: ";

QByteArray recvaddr;
QString subject;
QString content;

QByteArray recvdata;
};

#endif // SMTP_H
