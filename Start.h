#pragma once

#include <QObject>
#include <QThread>
#include <QString>


#include <Windows.h>
#include <iostream>
#include <io.h>
#include <string>

#include "HTTP.h"
using namespace std;
static QString threadWorking;
class Start : public QObject
{
    Q_OBJECT


public:
    Start(QString dir, QObject *parent);
    ~Start();
    static void stsworkProcess(int a,int b);
    static void dlworking(QString txt);
public:

private:
    //static Start *thisInstance;

    QThread *workProcess = nullptr;
    QString dir;

private slots:
    void work();


private:

signals:
    void tstart();
signals:
    //void tworkError(int errorCode);
    void tworkProcess(int a,int b);
    void tworkFinished(bool done);

};
QString tNowWork();



