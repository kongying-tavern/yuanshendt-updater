#include "Start.h"
#include <QStringList>
#include <QtWidgets/QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QUrl>
#include <QtConcurrent>
#include <QFileInfo>

#include <qt_windows.h>
#include "file.h"
#include "MD5.h"
#include "Sandefine.h"
#include "Json.h"
#include "mainwindow.h"

Start *tp;

Start::Start(QString dir, QObject *parent)
    : QObject(parent)
{
    tp=this;
    qDebug()<<"Start被创建";
    tp->stlog(moduleStart,"Start被创建",NULL);
    http = new HTTP("","",NULL);//单线程下载

    connect(http, &HTTP::tworkMessageBox, this, &Start::tworkMessageBox);

    workProcess = new QThread;
    moveToThread(workProcess);
    connect(this, &Start::tstart, this, &Start::work);
    workProcess->start();
    this->dir = dir;

}

Start::~Start()
{
    tp->stlog(moduleStart,"Start被销毁",NULL);
    qDebug()<<"Start被销毁";
}

void Start::dlworking(LONG64 dlnow,LONG64 dltotal,void *tid,QString path)
{
    static QMutex mutex;
    mutex.lock();

    //qDebug()<<dlnow<<dltotal<<tid<<path;
    for(int i=0;i<maxDlThreah;++i)
    {
        //清理已完成的任务
        //if(netspeed[i].dl==netspeed[i].total && netspeed[i].total>0)
        if( netspeed[i].isdling && (netspeed[i].dl==netspeed[i].total && netspeed[i].total>0))
        {
            qDebug()<<&netspeed[i].tid<<"end dl"<<netspeed[i].path;
            netspeed[i].isdling=false;
            netspeed[i].tid=NULL;
            //netspeed[i].dl=0;
            //netspeed[i].hisDl=0;
            //netspeed[i].total=0;
            //netspeed[i].dlt=0;
            //netspeed[i].hisDlt=netspeed[i].dlt;
            //netspeed[i].path="";
            //emit tp->updataDlingmag();//强制更新进度文本
        }
    }
    for(int i=0;i<maxDlThreah;++i)
    {
        //qDebug()<<i<<netspeed[i].tid<<tid;
        if(netspeed[i].isdling==false)
        {
            //新下载线程开始
            qDebug()<<&tid<<"new dl"<<path;
            netspeed[i].isdling=true;
            netspeed[i].tid=tid;
            netspeed[i].dl=dlnow;
            netspeed[i].dlt=QDateTime::currentDateTime().toMSecsSinceEpoch();
            netspeed[i].hisDl=dlnow;
            netspeed[i].hisDlt=netspeed[i].dlt;
            netspeed[i].total=dltotal;
            if(path!="")netspeed[i].path=path;
            break;
        }
        if(netspeed[i].tid==tid && netspeed[i].isdling)
        {
            //正在下载的线程
            //qDebug()<<"update netspeed"<<tid<<dlnow<<dltotal<<path<<netspeed[i].path;
            netspeed[i].dl=dlnow;
            netspeed[i].dlt=QDateTime::currentDateTime().toMSecsSinceEpoch();
            netspeed[i].total=dltotal;
            break;
        }
    }
    mutex.unlock();
}
void Start::stlog(int module,QString str,int mod=NULL)
{
    if(tp) emit tp->log(module,str,mod);
}
void Start::dldone()
{
    emit tworkProcess(doneFile++,totalFile);
    tp->stlog(moduleStart,"下载完成 "+
              QString::number(doneFile)+"/"+
              QString::number(totalFile)
              ,NULL);
}
QString Start::tNowWork()
{
    qDebug()<<"update speed text";
    //return "";
    qint64 DETLAms;
    int p;
    LONG64 s;
    QString tem="";
    QString tems="0.00B/s";
    for(int i=0;i<maxDlThreah;++i)
    {
        if(netspeed[i].path!="")
        {
            //qDebug()<<"--------------";
            //计算已下载的百分比
            DETLAms=netspeed[i].dlt-netspeed[i].hisDlt;
            //qDebug().noquote()<<i<<netspeed[i].path<<netspeed[i].dl<<netspeed[i].hisDl<<DETLAms;
            p = (int)(100*((double)netspeed[i].dl/(double)netspeed[i].total));
            if(p<0)p=0;
            //qDebug().noquote()<<p;
            //下载速度格式化
            //s = (netspeed[i].dl-netspeed[i].hisDl)*2;
            s=(int)((double)(netspeed[i].dl-netspeed[i].hisDl)*((double)1000/(double)DETLAms));
            if(s>0)tems=conver(s);
            //qDebug().noquote()<<tems;
            //文本构造
            tem+=QString::number(p)+"%\t | "
                 +QString(tems)+"\t | "
                 +netspeed[i].path
                    ;
            //字节数缓存
            netspeed[i].hisDl=netspeed[i].dl;
            netspeed[i].hisDlt=netspeed[i].dlt;
            p=0;

            if(i<2)tem+="\n";
        }
        tems="0.00B/s";
    }
    //qDebug().noquote()<<tem;
    QCoreApplication::processEvents();
    return tem;
}
void Start::updaterErr()
{
    tp->stlog(moduleStart,"自动更新失败",NULL);
    emit tworkProcess(0,1);
    emit this->changePBText("自动更新失败,请单击重试");
    emit tworkFinished(false);
    emit this->changeMainPage(1,false);
    emit tworkMessageBox(1,
                         "自动更新失败",
                         uperr_new);
}
void Start::stopWork()
{
    tp->stlog(moduleStart,"任务线程被要求终止",NULL);
    if(tpoolhttp->activeThreadCount()>0)
    {
        tp->stlog(moduleStart,"停止下载线程池",NULL);
        qDebug()<<"停止线程池";
        tpoolhttp->thread()->terminate();
        this->thread()->terminate();
    }
}
void Start::work()
{

    QString path=this->dir;
    //Start::updaterErr();
    //return;
    emit this->changeMainPage(0);
    qDebug()<<"工作目标："<<path;
    tp->stlog(moduleStart,"工作目标："+path,NULL);
    //return;
    //return;
    /*前期工作********************************************************/
    /*互斥原神本体路径*/
    QFileInfo yuanshen(path+"/YuanShen.exe");
    if(yuanshen.exists())
    {
        qDebug()<<"目标目录找到原神本体";
        tp->stlog(moduleStart,"目标目录找到原神本体",NULL);
        emit this->changeProgressBarColor(
                    QString("rgb(255, 0, 0)")
                    ,QString("rgb(255, 0, 0)"));
        emit this->changeMainPage0label_Text("?");
        emit tworkMessageBox(
                    1,
                    "我受过专业的训练",
                    "但是请你不要把地图装在原神目录"
                    );
        emit this->changeMainPage(1,false);
        emit tworkFinished(false);
        tp->stlog(moduleStart,"线程主动退出",NULL);
        return;
    }
    /*获取系统环境变量temp*********************************************/
    emit this->changeMainPage0label_Text("初始化...");
    tp->stlog(moduleStart,"初始化...",NULL);
    emit tworkProcess(1,2);
    emit this->changeProgressBarColor(
                QString("rgb(235, 235, 235)")
                ,QString("rgb(58, 59, 64)"));
    QString tempPath;
    tempPath=getTempPath("temp");
    qDebug()<<"临时目录:"<<tempPath;
    tp->stlog(moduleStart,"临时目录:"+tempPath,NULL);
    /*在%temp%创建临时目录*/
    tempPath=tempPath+updaterTempDir;
    qDebug()<<"临时文件夹:"<<tempPath;
    tp->stlog(moduleStart,"创建临时文件夹"+tempPath,NULL);

    createFolderSlot(tempPath);
    /*在临时目录释放crt证书*/
    tp->stlog(moduleStart,"释放crt证书",NULL);
    httpcrt();

    /*获取在线md5******************************************************/
    /* 获取在线文件md5
     * url  download.yuanshen.site/md5.json
     * url  dlurl"md5.json"
     * path "md5.json"
     */
    tp->stlog(moduleStart,"\r\n获取在线文件MD5json",NULL);
    emit this->changeMainPage0label_Text("获取在线文件MD5...");
    isdling = 1;
    http->httpDownLoad(dlurl"md5.json","md5.json");
    isdling = 0;
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
    tp->stlog(moduleStart,"读入MD5文件",NULL);
    newMD5Str = readTXT(tempPath+"download/md5.json");
    qDebug()<<"开始转换成QSL";
    tp->stlog(moduleStart,"格式化MD5List",NULL);
    JSON *json=new JSON();
    json->tp=tp;
    json->jsonStr2QSL(newMD5Str,newFileList,newFileMD5);

    //    return;
    /*按需读取本地文件MD5**************************************************/
    emit tworkProcess(1,1000);//进度条归零
    QStringList needUpdate;
    QStringList needUpdateMD5;
    qDebug()<<"按需读取本地文件MD5:"<<path;
    emit this->log(moduleStart,"按需读取本地文件MD5",NULL);
    QString omd5;
    for(int i = 0; i< newFileList.size();++i)
    {
        //qDebug()<<newFileMD5.at(i)<<this->dir+"/"+newFileList.at(i);
        omd5 = getFlieMD5(this->dir+"/"+newFileList.at(i));
        //emit log(moduleMD5,"本地文件MD5:"+omd5+"|"+newFileList.at(i),NULL);
        emit log(moduleMD5,"本地文件:\t"+newFileList.at(i),NULL);
        emit log(moduleMD5,"本地MD5:\t"+omd5,NULL);
        emit tworkProcess(i,newFileList.size());
        emit this->changeMainPage0label_Text("正在扫描本地文件MD5: "
                    +QString::number(i)
                    +"|"
                    +QString::number(newFileList.size())
                    +"\n"+newFileList.at(i)
                                             );
        if(newFileMD5.at(i) == omd5)
        {
            emit log(moduleMD5,"云端MD5\t"+newFileMD5.at(i),NULL);
        }else{
            qDebug()<<"MD5不匹配:"<<dir+"/"+newFileList.at(i);
            emit log(moduleMD5,"云端MD5\t"+newFileMD5.at(i)+"\t需要更新",NULL);
            needUpdate<<newFileList.at(i);
            needUpdateMD5<<newFileMD5.at(i);
        }
        emit log(moduleMD5,"\r\n",NULL);
    }
    //return;

    /*下载需要更新的文件**********************************************/
    /* 下载文件
     * 需要更新的文件在 QStringList needUpdater
     * 下载文件前需要对字符串做很多工作
     * 一是反斜杠转斜杠并删除第一个斜杠
     */
    emit tworkProcess(0,1);//进度条归零
    emit log(moduleStart,"根据本地文件MD5下载需要更新的文件",NULL);
    QString tem;
    emit this->changeProgressBarColor(
                QString("#3880cc")
                ,QString("#00c500"));
    int retry=0;//多线程下载如何重试QAQ
    totalFile=needUpdate.size();
    doneFile=0;
    //初始libcurl线程池
    emit log(moduleStart,"初始libcurl线程池",NULL);
    emit log(moduleStart,"设置同时下载任务数为"+QString::number(maxDlThreah),NULL);
    tpoolhttp=QThreadPool::globalInstance();
    tpoolhttp->setMaxThreadCount(maxDlThreah);
    //tpoolhttp->setMaxThreadCount(needUpdate.size());//A8:我tm谢谢你
    tpoolhttp->setExpiryTimeout(-1);
    isdling = 1;
    for(int i = 0; i< needUpdate.size();++i)
    {
        if(needUpdateMD5.at(i)!=getFlieMD5(tempPath+"download/Map/"+needUpdate.at(i)))
        {
            qDebug()<<"全新下载";
            emit log(moduleStart,"新建下载任务\t"+needUpdate.at(i),NULL);
            //构造下载链接
            QString url=dlurlMap+QUrl::toPercentEncoding(needUpdate.at(i));
            QString dlpath="Map/"+QString(needUpdate.at(i));

            thttp = new HTTP(url,dlpath,this);
            connect(thttp,&HTTP::dldone
                    ,this,&Start::dldone
                    ,Qt::DirectConnection
                    );
            connect(thttp, &HTTP::tworkMessageBox
                    , this, &Start::tworkMessageBox
                    ,Qt::DirectConnection
                    );
            tpoolhttp->start(thttp);

        }else{
            qDebug()<<needUpdate.at(i)<<"已下载";
            tp->stlog(moduleStart,"文件已被下载\t"+needUpdate.at(i),NULL);
            Start::dldone();
        }

    }

    //tpoolhttp->activeThreadCount();
    tpoolhttp->dumpObjectTree();
    tpoolhttp->waitForDone(-1);
    isdling = 0;
    tpoolhttp->clear();
    qDebug()<<"下载完成";
    tp->stlog(moduleStart,"下载完成",NULL);
    //qDebug()<<FindWindowW(NULL,(LPCWSTR)QString("「空荧酒馆」原神地图").unicode());//地图进程窗口

    //return;
    /*移动文件至目标目录*********************************************/
    /*
      * 移动文件
      * 下载好的地图存在%temp%\download\Map\
      * 移动的目标文件夹为path
      */
    tp->stlog(moduleStart,"正在移动文件",NULL);
    emit this->changeProgressBarColor(
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

        qDebug()<<QString::number(i+1)+
                  "|"+
                  QString::number(needUpdate.size())
                  ;
        emit this->changeMainPage0label_Text(
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
        }
        emit tworkProcess(i,needUpdate.size());
    }
    emit tworkProcess(1,1);
    emit this->changeMainPage0label_Text("不存在的看不到这句话的");
    emit this->changeMainPage(1,true);
    emit tworkFinished(true);
    tp->stlog(moduleStart,"更新流程结束",NULL);
}



