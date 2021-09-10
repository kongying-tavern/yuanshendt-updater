﻿#include "Start.h"
#include <QStringList>
#include <QtWidgets/QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QtConcurrent>
#include <QFileInfo>


#include "file.h"
#include "MD5.h"
#include "Sandefine.h"
#include "Json.h"
#include "mainwindow.h"


Start::Start(QString dir, QObject *parent)
    : QObject(parent)
{
    http = new HTTP("","",NULL);
    connect(this, &Start::tstart, this, &Start::work);
    connect(http, &HTTP::tworkMessageBox, this, &Start::tworkMessageBox);
    //connect(http, &HTTP::dldone, this, &Start::dldone);
    //thisInstance = this;
    workProcess = new QThread;
    moveToThread(workProcess);
    workProcess->start();
    this->dir = dir;

}

Start::~Start()
{

}

void Start::dlworking(LONG64 dlnow,LONG64 dltotal,void *tid,QString path)
{

    //qDebug()<<tid<<path<<dlnow<<dltotal;
    //qDebug()<<tid<<&tid<<path;
    for(int i=0;i<3;++i)
    {
        //清理已完成的任务
        if(netspeed[i].dl==netspeed[i].total && netspeed[i].total>0)
        {

            qDebug()<<"end dl in"<<netspeed[i].tid<<netspeed[i].path;
            netspeed[i].tid=NULL;
            netspeed[i].dl=0;
            netspeed[i].hisDl=0;
            netspeed[i].total=0;
            netspeed[i].path="";
            //emit dldone();
        }
    }
    for(int i=0;i<3;++i)
    {

        if(netspeed[i].tid==NULL && path!="")
        {
            //新下载线程开始
            qDebug()<<"new dl in"<<tid<<path;
            netspeed[i].tid=tid;
            netspeed[i].path=path;

            break;
        }
        if(netspeed[i].tid==tid)
        {
            //正在下载的线程
            netspeed[i].dl=dlnow;
            netspeed[i].total=dltotal;
            break;
        }

    }

}
void Start::dldone()
{
    emit tworkProcess(doneFile++,totalFile);
}
QString tNowWork(int &a,int &b)
{
    //return "";
    int p;
    LONG64 s;
    QString tem="";
    QString tems="0.00B/s";
    for(int i=0;i<3;++i)
    {
        if(netspeed[i].path!="" && netspeed[i].dl<netspeed[i].total)
        {
            //计算已下载的百分比
            p = (int)(100*((double)netspeed[i].dl/(double)netspeed[i].total));
            if(p<0)p=0;
            //下载速度格式化
            s = (netspeed[i].dl-netspeed[i].hisDl)*2;
            if(s>0)tems=conver(s);
            //下载信息构造
            tem+=QString::number(p)+"% | "
                 +tems+" | "
                 +netspeed[i].path
                 ;
            //下载字节数缓存
            netspeed[i].hisDl=netspeed[i].dl;

            p=0;
            tems="0.00B/s";
            if(i<2)tem+="\n";
        }
    }
    a=doneFile;
    b=totalFile;
    return tem;
}
void Start::updaterErr()
{
    emit tworkProcess(0,1);
    MainWindow::mutualUi->changePBText("自动更新失败,请单击重试");
    emit tworkFinished(false);
    MainWindow::mutualUi->changeMainPage(1,false);
    emit tworkMessageBox(1,
                         "自动更新失败",
                         uperr);
}
void Start::stopWork()
{
    if(tpoolhttp->activeThreadCount()>0)
    {
        qDebug()<<"停止线程池";
        tpoolhttp->thread()->terminate();
    }
}
void Start::work()
{

    QString path=this->dir;
    //Start::updaterErr();
    //return;
    MainWindow::mutualUi->changeMainPage(0);
    qDebug()<<"工作目标："<<path;
    //return;
    //return;
    /*前期工作********************************************************/
    /*互斥原神本体路径*/
    QFileInfo yuanshen(path+"/YuanShen.exe");
    if(yuanshen.exists())
    {
        qDebug()<<"目标目录找到原神本体";
        MainWindow::mutualUi->changeProgressBarColor(
                    QString("rgb(255, 0, 0)")
                    ,QString("rgb(255, 0, 0)"));
        MainWindow::mutualUi->changeMainPage0label_Text("?");
        emit tworkMessageBox(
                    1,
                    "我受过专业的训练",
                    "但是请你不要把地图装在原神目录"
                    );
        MainWindow::mutualUi->changeMainPage(1,false);
        emit tworkFinished(false);
        return;
    }
    /*获取系统环境变量temp*********************************************/
    MainWindow::mutualUi->changeMainPage0label_Text("初始化...");
    emit tworkProcess(1,2);
    MainWindow::mutualUi->changeProgressBarColor(
                QString("rgb(235, 235, 235)")
                ,QString("rgb(58, 59, 64)"));
    QString tempPath;
    tempPath=getTempPath("temp");
    qDebug()<<"临时目录:"<<tempPath;

    /*在%temp%创建临时目录*/
    tempPath=tempPath+updaterTempDir;
    qDebug()<<"临时文件夹:"<<tempPath;
    createFolderSlot(tempPath);
    /*在临时目录释放crt证书*/
    httpcrt();

    /*获取在线md5******************************************************/
    /* 获取在线文件md5
     * url  download.yuanshen.site/md5.json
     * url  dlurl"md5.json"
     * path "md5.json"
     */

    MainWindow::mutualUi->changeMainPage0label_Text("获取在线文件MD5...");
    http->httpDownLoad(dlurl"md5.json","md5.json");

    /*读取在线文件md5.json到
      * QJson  newmd5.json
      * 字符串  QString newMD5Str
      * 文件数  QSL newFileList
      * 文件MD5 QSL newFileMD5
      */
    //_sleep(1000);
    QString newMD5Str;
    QStringList newFileList;
    QStringList newFileMD5;
    //qDebug()<<tempPath<<"download/md5.json";
    newMD5Str = readTXT(tempPath+"download/md5.json");
    qDebug()<<"开始转换成QSL";
    jsonStr2QSL(newMD5Str,newFileList,newFileMD5);

//    return;
    /*按需读取本地文件MD5**************************************************/
    emit tworkProcess(0,1);//进度条归零

    QStringList needUpdate;

    qDebug()<<"按需读取本地文件MD5:"<<path;

    for(int i = 0; i< newFileList.size();++i)
    {
        //qDebug()<<newFileMD5.at(i)<<this->dir+"/"+newFileList.at(i);
        emit tworkProcess(i,newFileList.size());
        if(newFileMD5.at(i) != getFlieMD5(this->dir+"/"+newFileList.at(i)))
        {
            MainWindow::mutualUi->changeMainPage0label_Text("正在扫描本地文件MD5:"+newFileList.at(i));
            qDebug()<<"MD5不匹配:"<<dir+"/"+newFileList.at(i);
            needUpdate<<newFileList.at(i);
        }
    }
    //return;

    /*下载需要更新的文件**********************************************/
    /* 下载文件
     * 需要更新的文件在 QStringList needUpdater
     * 下载文件前需要对字符串做很多工作
     * 一是反斜杠转斜杠并删除第一个斜杠
     */
     QString tem;
     MainWindow::mutualUi->changeProgressBarColor(
                 QString("#3880cc")
                 ,QString("#00c500"));
     int retry=0;//多线程下载如何重试QAQ
     totalFile=needUpdate.size();
     doneFile=0;
     //初始libcurl线程池
     tpoolhttp=QThreadPool::globalInstance();
     tpoolhttp->setMaxThreadCount(3);
     tpoolhttp->setExpiryTimeout(-1);

     for(int i = 0; i< needUpdate.size();++i)
     {

         //qDebug()<<"开始下载:"<<needUpdate.at(i);
         //构造下载链接
         QString url=dlurlMap+QUrl::toPercentEncoding(needUpdate.at(i));
         QString dlpath="Map/"+QString(needUpdate.at(i));
         //qDebug()<<"downloadurl :"<<url;
         //qDebug()<<"downloadpath:"<<dlpath;

         thttp = new HTTP(url,dlpath,this);
         connect(thttp,&HTTP::dldone,this,&Start::dldone,Qt::DirectConnection);
         tpoolhttp->start(thttp);

         //QThread::sleep(1);
     }

     //tpoolhttp->activeThreadCount();
     tpoolhttp->waitForDone(-1);
     tpoolhttp->clear();
     qDebug()<<"下载完成";
     //MainWindow::mutualUi->changeMainPage0label_Text("下载完成");
     /*移动文件至目标目录*********************************************/
     /*
      * 移动文件
      * 下载好的地图存在%temp%\download\Map\
      * 移动的目标文件夹为path
      */
    //MainWindow::mutualUi->changeMainPage0label_Text("正在移动文件");
    MainWindow::mutualUi->changeProgressBarColor(
                QString("#00c500")
                ,QString("#078bff"));
    int f=0;
    for(int i = 0; i< needUpdate.size();++i)
    {




        //构造新旧文件名
        QString oldPath=getTempPath("temp")+
                updaterTempDir+
                "download/"+"Map/"+
                QString(needUpdate.at(i))
                ;
        QString newPath=path+
                "/"+
                QString(needUpdate.at(i))
                ;
        //qDebug()<<"oldPath:"<<oldPath;
        //qDebug()<<"newPath:"<<newPath;
        qDebug()<<QString::number(i+1)+
                  "|"+
                  QString::number(needUpdate.size())
                  ;
        MainWindow::mutualUi->changeMainPage0label_Text(
                    QString::number(i+1)+
                    "|"+
                    QString::number(needUpdate.size())+
                    " 正在移动文件:"+
                    needUpdate.at(i)
                    );
        QFileInfo info(newPath);
        createFolderSlot(info.path());

        if(moveFile(oldPath,newPath))
        {
            qDebug()<<"√（＾－＾）";
            f=0;
        }else{
            qDebug()<<"移动失败第"<<f<<"次";
            f++;
            i--;
            if(f>1)
            {
                Start::updaterErr();
                return;
            }

            //QMessageBox::warning(NULL,"不对劲","尝试移动\n"+needUpdate.at(i)+"\n的时候遇到了蹦蹦炸弹都解决不了的问题");
        }
        emit tworkProcess(i,needUpdate.size());
    }
    emit tworkProcess(1,1);
    MainWindow::mutualUi->changeMainPage0label_Text("不存在的看不到这句话的");
    MainWindow::mutualUi->changeMainPage(1,true);
    emit tworkFinished(true);
}



