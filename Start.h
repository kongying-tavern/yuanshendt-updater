#pragma once

#include <QObject>
#include <QThread>
#include <QThreadPool>
#include <QString>


#include <Windows.h>

#include <iostream>
#include <io.h>
#include <string>

#include "HTTP.h"
using namespace std;
static QString threadWorking;

struct stnetspeed
{
    bool isdling=false;
    LONG64 hisDl;
    qint64 hisDlt;
    LONG64 dl;
    qint64 dlt;
    LONG64 total;
    void *tid;
    QString path;
};
static stnetspeed netspeed[3];
static int totalFile;
static int doneFile;
class Start : public QObject
{
    Q_OBJECT


public:
    Start(QString dir, QObject *parent);
    ~Start();
    void updaterErr();
    void relaytworkMessageBox(int tag,QString title,QString txt);
    void stopWork();
public:
    static Start *thisInstance;
    static void dlworking(LONG64 dlnow,LONG64 dltotal,void *tid,QString path);
    static void stlog(int module,QString str,int mod);
private:

    QThread *workProcess = nullptr;
    QThreadPool *tpoolhttp = nullptr;
    QString dir;
    HTTP *http=nullptr;
    HTTP *thttp=nullptr;



private slots:
    void work();
    void dldone();
public slots:

signals:
    void tstart();
    void log(int module,QString str,int mod);
signals:
    //void tworkError(int errorCode);
    void tworkProcess(int a,int b);
    void tworkFinished(bool done);
    void tworkMessageBox(int tag,QString title,QString txt);


};
QString tNowWork();



