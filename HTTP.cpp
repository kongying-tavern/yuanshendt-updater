#include "HTTP.h"
#include <QFile>
#include <QBuffer>
#include <QDebug>
#include <QString>
#include <QFileInfo>
#include <iostream>
#include <string.h>
#define CURL_STATICLIB
#include "curllib\curl.h"
#include "file.h"
#include <stdio.h>
#include <Sandefine.h>
#include <stddef.h>
#include <QtWidgets/QMessageBox>
using namespace std;

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
    QString tem;
    //tem = QString::fromStdString(getTempPath("temp"))+updaterTempDir+"download\\";
    //createFolderSlot(tem.toStdString());
    //qDebug()<<tem+Path;
    QFileInfo info(QString::fromStdString(getTempPath("temp"))+updaterTempDir+"download\\"+Path);
    qDebug()<<"即将下载文件:"<<URL;
    qDebug()<<"准备创建目录:"<<info.path().replace("/","\\");
    qDebug()<<"即将打开文件:"<<QString::fromStdString(getTempPath("temp"))+updaterTempDir+"download\\"+Path;
    createFolderSlot(info.path());
    //return 0;

    //创建文件准备写入
    //FILE *pagefile= fopen((QString::fromStdString(getTempPath("temp"))+updaterTempDir+"download\\"+Path).toStdString().c_str(), "wb");
    FILE *pagefile=NULL;
    int err;
    err = fopen_s(&pagefile
                      ,(QString::fromStdString(getTempPath("temp"))+updaterTempDir+"download\\"+Path).toStdString().c_str()
                      ,"wb"
                      );
    qDebug()<<"打开的文件句柄"<<pagefile<<"|"<<err;
    //Sleep(10000);


    //初始化curl    
    CURL *handle = curl_easy_init();


    //SSL双向认证准备工作
    QString resProfiex="CRT";//资源文件前缀
    QString resFile ="Resource/crt/curl-ca-bundle.crt";//资源文件中的文件名称
    tem = QString::fromStdString(getTempPath("temp"))+updaterTempDir+"\\crt";
    createFolderSlot(tem);//创建文件夹保存crt
    tem = QString::fromStdString(getTempPath("temp"))+updaterTempDir+"\\crt\\crt.crt";
    QString saveFile=tem;//要保存文件的全路径名称
    saveResourecFile(resProfiex,resFile,saveFile);//写出资源文件中的crt


    URL.replace(" ","%20");//A8我被你的带空格文件名坑得好惨啊!!!!!!!!!!X2!
    curl_easy_setopt(handle, CURLOPT_URL, URL.toStdString().c_str());//指定网址
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 1L);//删除控制台消息
    //curl_easy_setopt(handle, CURLOPT_HEADER, 1);    //需要header头
    //curl_easy_setopt(handle, CURLOPT_NOBODY, 1);    //不需要body
    curl_easy_setopt(handle,CURLOPT_FOLLOWLOCATION,1);//设置跟随重定向
    curl_easy_setopt(handle,CURLOPT_MAXCONNECTS,20);//设置最大连接数

    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 1L);//openssl编译时使用curl官网或者firefox导出的第三方根证书文件
    curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(handle, CURLOPT_CAINFO, tem.toStdString().c_str());/* 证书路径 */
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data);//收到数据反调
    int curlreint=curl_easy_perform(handle);
    if (curlreint == CURLE_OK)
    {
        if (pagefile)
        {
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, pagefile);
            curl_easy_perform(handle);//
            reint = fclose(pagefile);            
            if(reint!=0)QMessageBox::warning(NULL,"写出文件","错误:"+QString::number(reint));
        }else{
            reint = -2;
        }
        reint = 0;
    } else {        
        QMessageBox::warning(NULL,"下载文件","错误:"+QString::number(curlreint));
        reint = -3;
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
