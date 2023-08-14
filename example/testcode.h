#ifndef TESTCODE_H
#define TESTCODE_H

#include <QObject>
#include <QQuickItem>
class Voronoi : public QObject
{
    Q_OBJECT
public:
    explicit Voronoi(QObject *parent = nullptr);

    Q_INVOKABLE void test();//成员函数
    Q_INVOKABLE double add(double x, double y);//成员函数
};

#endif // TESTCODE_H
