#include "Start.h"
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
    workProcess = new QThread;
    moveToThread(workProcess);
    workProcess->start();
    this->dir = dir;
    connect(this, &Start::tstart, this, &Start::work);
}

Start::~Start()
{

}


void Start::dlworking(LONG64 dlnow,LONG64 dltotal)
{
    //qDebug()<<"dlnow/dltotal"<<a1<<b;
    a1=dlnow;
    b=dltotal;
    //threadWorking = txt;
}
QString tNowWork()
{
    //qDebug()<<a1<<a2<<b;
    if(b==0)
    {
        return "";
    }
    //qDebug()<<"计算网速";
    int p = (int)(100*((double)a1/(double)b));
    //qDebug()<<p;
    QString tem = conver((a1-a2)*2);
    a2=a1;

    return QString::number(p)+"%|"+tem;
}

void Start::work()
{
    a1=0;
    a2=0;
    b=0;

    //mutualStart = this;
    QString path=this->dir;

    //return;
    MainWindow::mutualUi->changeMainPage(0);
    qDebug()<<"工作目标："<<path;
    //return;
    //return;
    /*前期工作********************************************************/
    /*获取系统环境变量temp*/
    MainWindow::mutualUi->changeMainPage0label_Text("初始化...");
    emit tworkProcess(1,2);
    MainWindow::mutualUi->changeProgressBarColor(
                QString("rgb(235, 235, 235)")
                ,QString("rgb(58, 59, 64)"));
    QString tempPath;
    tempPath=getTempPath("temp");
    qDebug()<<"临时目录:"<<tempPath;
    /*
    cout<<url<<endl;
    cout<<urlMap<<endl;
    */
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
    httpDownLoad(dlurl"md5.json","md5.json");

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
    for(int i = 0; i< needUpdate.size();++i)
    {

        //qDebug()<<"开始下载:"<<needUpdate.at(i);
        //构造下载链接
        QString url=dlurlMap+QUrl::toPercentEncoding(needUpdate.at(i));
        QString dlpath="Map/"+QString(needUpdate.at(i));
        qDebug()<<"downloadurl :"<<url;
        qDebug()<<"downloadpath:"<<dlpath;

        //QMessageBox::information(NULL,QString::number(i),dlpath);
        a1=0;
        a2=0;
        b=0;
        MainWindow::mutualUi->changeMainPage0label_Text("下载需要更新的文件:"+needUpdate.at(i));
        emit tworkProcess(i,needUpdate.size());

        httpDownLoad(url,dlpath);

        //Sleep(100);

    }
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
            if(f>3)
            {
                Sleep(1000);
                emit tworkProcess(0,1);
                MainWindow::mutualUi->changePBText("自动更新失败,请单击重试");
                emit tworkFinished(false);
                MainWindow::mutualUi->changeMainPage(1,false);
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



