#pragma once

#include <QObject>
#include <QThread>
#include <QString>


#include <Windows.h>
#include <iostream>
#include <io.h>
#include <string>

using namespace std;

class Start : public QObject
{
    Q_OBJECT

public:
    Start(QString dir, QObject *parent);
    ~Start();
    static void stsworkProcess(int a,int b);

private:
    QThread *workProcess = nullptr;
    QString dir;

private slots:
    void work();

signals:
    void tstart();
signals:
    //void tworkError(int errorCode);
    void tworkProcess(int a,int b);
    void tworkFinished(bool done);
};




