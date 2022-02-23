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

#define maxDlThreah 3
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
static stnetspeed netspeed[maxDlThreah];
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
signals:
    /*窗口控制托管信号*/
    void changeMainPage(int page,bool done=false);/*修改MainPage,0:进度条 1:选择目录按钮*/
    void changeMainPage0label_Text(QString str);/*修改MainPage->Label_Text文本*/
    void changeProgressBarValue(int a,int b);/*修改进度条进度*/
    void changeProgressBarColor(QString rgba,QString rgbb);
    void changePBText(QString str);//修改按钮文本
    void updataDlingmag();//强制刷新进度文本
};
QString tNowWork();



