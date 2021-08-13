#include "HTTP.h"
#include <QFile>
#include <QBuffer>
#include <QDebug>
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
HTTP::HTTP()
{

}
HTTP::~HTTP()
{

}
void httpcrt()
{

    crtPath=getTempPath("temp")+updaterTempDir+"crt/crt.crt";
    QFileInfo info(crtPath);
    saveResourecFile("CRT",                             //资源文件前缀
                     "Resource/crt/curl-ca-bundle.crt", //资源文件路径
                     crtPath                            //资源文件即将释放到的路径
                     );//写出资源文件中的crt
}
//下载文件
int httpDownLoad(QString URL,QString Path)
{

    int reint = -10;
    //qDebug()<<"OpeningURL:"<<URL<<Path;
    //return 0;

    //URL=dlurl"md5.json";
    //Path="md5.json";
    /* 准备下载临时文件     *
     * 下载的文件将保存在临时目录/download/
     * 文件名 Path
     */


    QFileInfo info(getTempPath("temp")+updaterTempDir+"download/"+Path);
    qDebug()<<"即将下载文件:"<<URL;
    qDebug()<<"准备创建目录:"<<info.path();
    qDebug()<<"即将打开文件:"<<getTempPath("temp")+updaterTempDir+"download/"+Path;
    createFolderSlot(info.path());
    //return 0;

    //创建文件准备写入

    FILE *pagefile=NULL;
    int err;
    err = fopen_s(&pagefile,
                  (getTempPath("temp")+updaterTempDir+"download/"+Path).toStdString().c_str()
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
    qDebug()<<"打开的文件句柄"<<&pagefile<<"|"<<err;
    if(err!=0)
    {
        fclose(pagefile);
        return err;
    }
    //Sleep(10000);


    //初始化curl    
    CURL *handle = curl_easy_init();


    URL.replace(" ","%20");//A8我被你的带空格文件名坑得好惨啊!!!!!!!!!!X2!
    curl_easy_setopt(handle, CURLOPT_URL, URL.toStdString().c_str());//指定网址    
    //curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //需要header
    //curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body
    curl_easy_setopt(handle,CURLOPT_FOLLOWLOCATION,1);//设置跟随重定向
    curl_easy_setopt(handle,CURLOPT_MAXCONNECTS,20);//设置最大连接数
    curl_easy_setopt(handle, CURLOPT_FAILONERROR, 1L);//不下载>400页面
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);//获取进度信息
    curl_easy_setopt(handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);//openssl编译时使用curl官网或者firefox导出的第三方根证书文件
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(handle, CURLOPT_CAINFO, crtPath.toStdString().c_str());/* 证书路径 */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);//收到数据反调
    if(pagefile)
    {
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, pagefile);
        int curlreint=curl_easy_perform(handle);
        reint = fclose(pagefile);
        qDebug()<<"fclose reint "<<reint;
        if(curlreint == CURLE_OK)
        {
            return curlreint;
        }else{
            QMessageBox::warning(NULL,"下载文件","错误:"+QString::number(curlreint));
            reint = -3;
        }
        if(reint!=0){
            qDebug()<<"closeflie?.?"<<reint;
        }

    }

    curl_easy_cleanup(handle);
    //curl_global_init(CURL_GLOBAL_ALL);
    //curl_global_cleanup();
    return reint;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{

    FILE * pFile = (FILE *)stream;
    size_t retSize;
    retSize = fwrite(ptr,size,nmemb,pFile);
    //cout<<size*nmemb<<"bytes"<<" received!"<<endl;
    fflush(pFile);
    //return retSize;
    return nmemb;

}
int progress_callback(void *clientp,//用户自定义参数,通过设置CURLOPT_XFERINFODATA属性来传递
                      curl_off_t dltotal,//需要下载的字节数
                      curl_off_t dlnow,//已经下载的字节数
                      curl_off_t ultotal,//将要上传的字节数
                      curl_off_t ulnow//已经上传的字节数
                      )//返回非0将会中断传输，错误代码是 CURLE_ABORTED_BY_CALLBACK
{
    QString tem;


    Start::dlworking(dlnow,dltotal);

    //Start &mutualStart = Start::getInstance();
    //Start *nowStart=(Start*)(Start::mutualStart);
    //nowStart->tworkDlnow(tem);
    //mutualStart.tworkDlnow(tem);

    return 0;
}
QString conver(LONG64 l)
{

    if(l<1024)
    {
        return QString::number(l,'f',2)+"B/s";
    }else if(l>1024)
    {
        return QString::number(l/1024,'f',2)+"KiB/s";
    }else if(l>(1024*1024))
    {
        return QString::number(l/(1024*1024),'f',2)+"MiB/s";
    }
    return "";
}
