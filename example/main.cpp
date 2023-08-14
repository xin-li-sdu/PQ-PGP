//#include <QGuiApplication>
#include "qfont.h"
#pragma GCC optimize(2)
#include <QApplication>
#include <QtCore>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QProcess>
#include <QQmlEngine>
#include <QQmlContext>
#include <QObject>
#include <NTL/ZZ.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <QChar>
#include <time.h>
//#include <Windows.h>
//#include <ShellAPI.h>
#include <QThread>
#include "ENC_SIGN.h"
#include "api.h"
#include "params.h"
GNTRU_MSK_Data * GNTRU_MSKD ;
GNTRU_MPK_Data * GNTRU_MPKD ;
char MPK1[10000];
char rMPK[10000];
int test;
QMap<QString, QVariant> properties(){
    QMap<QString, QVariant> map;
    //    map["installHelper"] = QVariant::fromValue(QVariant::fromValue(InstallHelper::getInstance()));
    return map;
}
//extern char msk='0';
char msk='0';
int main(int argc, char *argv[])
{
    test=0;
    srand((unsigned)time(NULL));
//    QCoreApplication::setOrganizationName("xin-li-sdu");
//    QCoreApplication::setOrganizationDomain("https://");
//    QCoreApplication::setApplicationName("PGP-TEST");
    QApplication::setOrganizationName("xin-li-sdu");
    QApplication::setOrganizationDomain("https://");
    QApplication::setApplicationName("PGP-TEST");
    QQuickStyle::setStyle("Basic");
   //QGuiApplication app(argc, argv);
    QApplication app(argc, argv);
    QQmlApplicationEngine engine;


    QFont ff("Times New Roman, Microsoft JhengHei, SimSun-ExtB",10);
    app.setFont(ff);

    qmlRegisterType<ENC_SIGN>("encsign",1,0,"ENC_SIGN");//加载的c++文件!!!

    QMapIterator<QString, QVariant> iterator(properties());
    while (iterator.hasNext()) {
        iterator.next();
        QString key = iterator.key();
        QVariant value = iterator.value();
        engine.rootContext()->setContextProperty(key,value);
    }
    engine.rootContext()->setContextProperty("properties",properties());
    const QUrl url(QStringLiteral("qrc:/App.qml"));

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

//    GNTRU_MSKD = new GNTRU_MSK_Data;
//    GNTRU_MPKD = new GNTRU_MPK_Data;
//    memset(MPK1, 0, 10000);
//    Api_Keygen(GNTRU_MPKD, GNTRU_MSKD, MPK1);
    //--------将keygen放到主线程之外-------//
            ZZ_p::init(q1);
            ApiKeygenThread *apiKeygenThread = new ApiKeygenThread();
            QThread *thread = new QThread();
            apiKeygenThread->moveToThread(thread);
            ApiKeygenThread::connect(thread, SIGNAL(started()), apiKeygenThread, SLOT(run()));
            ApiKeygenThread::connect(apiKeygenThread, SIGNAL(finished()), thread, SLOT(quit()));
            ApiKeygenThread::connect(apiKeygenThread, SIGNAL(finished()), apiKeygenThread, SLOT(deleteLater()));
            ApiKeygenThread::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
            thread->start();
     //--------将keygen放到主线程之外-------//
    return app.exec();
}
