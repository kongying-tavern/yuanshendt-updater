#include "HTTP.h"
#include <QFile>
#include <QBuffer>

#include <QString>
#include <QFileInfo>
#include <iostream>
#include <string.h>
#include "mainwindow.h"
#include "Start.h"
#include "file.h"
#include <stdio.h>
#include <Sandefine.h>
#include <stddef.h>
#include <QtWidgets/QMessageBox>
#include <QByteArray>
using namespace std;
//初始化


HTTP::HTTP(QString URL,QString Path,QObject *parent)
    : QObject(parent)
{

    this->turl=URL;
    this->tdlpath=Path;

}
HTTP::~HTTP()
{
    qDebug()<<&tid<<"HTTP被销毁";
}
void httpcrt()
{
    qDebug()<<"释放crt";
    crtPath=getTempPath("temp")+updaterTempDir+"crt/crt.crt";
    QFileInfo info(crtPath);
    saveResourecFile("CRT",                             //资源文件前缀
                     "Resource/crt/curl-ca-bundle.crt", //资源文件路径
                     crtPath                            //资源文件即将释放到的路径
                     );//写出资源文件中的crt
}
//下载文件
void HTTP::run()
{
    //qDebug()<<"HTTP run 线程ID：" << QThread::currentThread();
    if(this->turl=="" && this->tdlpath==""){
        qDebug()<<"无传参";
        return;
    }
    tid=QThread::currentThreadId();//这东西必须放在线程启动后,否则变单线程
    qDebug()<<&tid<<"HTTP被创建";
    qDebug()<<&tid<<"任务:"<<this->turl<<this->tdlpath;
    HTTP::httpDownLoad(this->turl,this->tdlpath);
}
int HTTP::httpDownLoad(QString URL,QString Path)
{
    /* 准备下载临时文件     *
     * 下载的文件将保存在临时目录/download/
     * 文件名 Path
     */
    int reint = -10;
    QFileInfo info(getTempPath("temp")+updaterTempDir+"download/"+Path);
    qDebug()<<&tid<<"URL:"<<URL;
    qDebug()<<&tid<<"准备创建目录:"<<info.path();
    qDebug()<<&tid<<"即将打开文件:"<<getTempPath("temp")+updaterTempDir+"download/"+Path;
    createFolderSlot(info.path());

    //创建文件准备写入
    pagefile=NULL;
    int err;
    err = fopen_s(&pagefile,
                  (
                      getTempPath("temp")
                      +updaterTempDir+"download/"
                      +Path
                  ).toStdString().c_str()
                  ,"wb"
                  );

    if(err!=0)
    {
        qDebug()<<"尝试用宽字符编码打开文件";
        fclose(pagefile);
        QString tem;
        tem=getTempPath("temp")+updaterTempDir+"download/"+Path;
        err = fopen_s(&pagefile,
                      tem.toLocal8Bit().constData(),
                      "wb"
                      );
    }
    qDebug()<<&tid<<"打开的文件"<<&pagefile<<"|"<<err;
    if(err!=0)
    {
        fclose(pagefile);
        return err;
    }
    //初始化curl    
    handle = curl_easy_init();
    qDebug()<<"创建的libcurl:"<<&handle;
    curl_easy_setopt(handle, CURLOPT_URL, URL.toStdString().c_str());//指定网址    
    //curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //需要header
    //curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body
    curl_easy_setopt(handle,CURLOPT_FOLLOWLOCATION,1);//设置跟随重定向
    curl_easy_setopt(handle,CURLOPT_MAXCONNECTS,20);//设置最大连接数
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);//不下载>400页面
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);//获取进度信息
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, this->progress_callback);//进度条
    curl_easy_setopt(handle,CURLOPT_XFERINFODATA,tid);//线程ID
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);//openssl编译时使用curl官网或者firefox导出的第三方根证书文件
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(handle, CURLOPT_CAINFO, crtPath.toStdString().c_str());//证书路径
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, this->write_data);//收到数据反调
    if(pagefile)//文件打开成功则开始下载
    {
        Start::dlworking(0,0,tid,Path);//通知Start线程本线程下载的文件
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, pagefile);//数据流定向至FILE *
        qDebug()<<&tid<<"下载开始";
        int curlreint=curl_easy_perform(handle);//开始下载
        reint = fclose(pagefile);
        if(curlreint == CURLE_OK)
        {
            qDebug()<<&tid<<"下载完成";
            emit dldone();
        }else{
            qDebug()<<&tid<<"下载文件错误"<<curlreint;
            emit tworkMessageBox(1
                                 ,"下载文件"
                                 ,"错误:"+QString::number(curlreint)
                                   +"\r\n"+Path
                                   +"\r\n"+curlerr2str(curlreint)
                                 );
        }
        if(reint!=0){
            qDebug()<<&tid<<"closeflie?.?"<<reint;
            qDebug()<<&tid<<"关闭文件时错误"<<reint;
            emit tworkMessageBox(1
                                 ,"关闭文件"
                                 ,"错误:"+QString::number(reint)
                                    +"\r\n"+Path
                                 );
        }
    }
    curl_easy_cleanup(handle);
    //curl_global_init(CURL_GLOBAL_ALL);
    //curl_global_cleanup();

    return reint;
}

size_t HTTP::write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{

    FILE * pFile = (FILE *)stream;
    size_t retSize;
    retSize = fwrite(ptr,size,nmemb,pFile);
    //cout<<size*nmemb<<"bytes"<<" received!"<<endl;
    fflush(pFile);
    return retSize;

}
int HTTP::progress_callback(void *clientp,//用户自定义参数,通过设置CURLOPT_XFERINFODATA属性来传递
                      curl_off_t dltotal,//需要下载的字节数
                      curl_off_t dlnow,//已经下载的字节数
                      curl_off_t ultotal,//将要上传的字节数
                      curl_off_t ulnow//已经上传的字节数
                      )//返回非0将会中断传输，错误代码是 CURLE_ABORTED_BY_CALLBACK
{
    Start::dlworking(dlnow,dltotal,clientp,"");//进度报告
    return 0;
}
QString conver(LONG64 l)//下载速度格式化
{
    QString restr;
    if(l<1024)
    {
        restr = QString::number(l,'f',2)+"B/s";
    }else if(1024<=l && l<(1024*1024))
    {
        restr = QString::number((double)l/(double)1024,'f',2)+"KiB/s";
    }else if((1024*1024)<=l)
    {
        restr = QString::number((double)l/(double)(1024*1024),'f',2)+"MiB/s";
    }
    //qDebug()<<QThread::currentThread()<<restr;
    return restr;
}
