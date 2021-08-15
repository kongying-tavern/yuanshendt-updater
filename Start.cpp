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
    /*遍历目录*/

    QStringList localFileList;//创建字符串数组
    /*读取本地文件MD5**************************************************/

    //qDebug()<<"遍历目录:"<<path;
    //return;
    file_search(path,localFileList);
    //return;
    emit tworkProcess(0,1);
    /*获取本地文件MD5*/
    QStringList localFileListMD5;
    MainWindow::mutualUi->changeProgressBarColor(
                QString("rgb(235, 235, 235)")
                ,QString("rgb(58, 59, 64)"));
    for(int i = 0; i< localFileList.size();++i)
    {

        MainWindow::mutualUi->changeMainPage0label_Text("正在扫描本地文件MD5:"+localFileList.at(i));
        emit tworkProcess(i,localFileList.size());

        //qDebug()<<localFileList.at(i);
        localFileListMD5<<getFlieMD5(localFileList.at(i));
    }

    /*
      旧文件列表     localFileList
      旧文件列表-MD5 localFileListMD5
    */
    /*获取在线md5******************************************************/
    /* 获取在线文件md5
     * url  download.yuanshen.site/md5.json
     * url  dlurl"md5.json"
     * path "md5.json"
     */

    MainWindow::mutualUi->changeMainPage0label_Text("获取在线文件MD5...");
    httpDownLoad(dlurl"md5.json","md5.json");

    /*读取在线文件md5.json到
      * QJson newmd5.json
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
    /*文件对比********************************************************/
    /*
      * 获取需要更新的文件
      * 旧文件列表     QStringList localFileList
      * 新文件列表     QStringList newFlieList
      * 旧文件列表-MD5 QStringList localFileListMD5
      * 新文件列表-MD5 QStringList newFileMD5
      */

    MainWindow::mutualUi->changeMainPage0label_Text("对比需要更新的文件");
    emit tworkProcess(0,1);
    Sleep(300);
    emit tworkProcess(1,1);
    MainWindow::mutualUi->changeProgressBarColor(
                QString("rgb(58, 59, 64)")
                ,QString("#3880cc"));
    QStringList needUpdate;
    needUpdate = getUptater(localFileList,
                            localFileListMD5,
                            newFileList,
                            newFileMD5,
                            path
                            );
    if(needUpdate.size()==0)
    {
        //此时文件均为最新
        qDebug()<<"无需更新";
        MainWindow::mutualUi->changeMainPage0label_Text("无需更新");
        MainWindow::mutualUi->changeMainPage(1,true);
        emit tworkFinished(true);
        return;
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
        }else{
            qDebug()<<"移动失败";
            //QMessageBox::warning(NULL,"不对劲","尝试移动\n"+needUpdate.at(i)+"\n的时候遇到了蹦蹦炸弹都解决不了的问题");
        }
        emit tworkProcess(i,needUpdate.size());
    }
    emit tworkProcess(1,1);
    MainWindow::mutualUi->changeMainPage0label_Text("所有工作结束,可关闭下载器");

    MainWindow::mutualUi->changeMainPage(1,true);

    emit tworkFinished(true);
}



