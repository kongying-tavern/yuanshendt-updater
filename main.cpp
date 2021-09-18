#include <QApplication>
#include "HTTP.cpp"
#include "MD5.cpp"
#include "file.cpp"
#include <QDebug>
#include <QString>
#include "Sandefine.h"
#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <io.h>
#include <string>

void MessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{

    qDebug().noquote()<<msg;//调试窗输出
    QJsonObject joc;
    QJsonArray jay;
    switch(type)
    {
    case QtDebugMsg:
        joc.insert("tag","Debug");
        break;
    case QtInfoMsg:
        joc.insert("tag","Info");
        break;
    case QtWarningMsg:
        joc.insert("tag","Warning");
        break;
    case QtCriticalMsg:
        joc.insert("tag","Critical");
        break;
    case QtFatalMsg:
        joc.insert("tag","FatalMsg");
        break;
    }
    // 设置输出json格式
    joc.insert("Time",
               QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())
               );
    joc.insert("File",context.file);
    joc.insert("Line",context.line);
    joc.insert("Function",context.function);
    joc.insert("Message",msg);

    static QMutex mutex;
    // 加锁
    mutex.lock();
    // 写日志
    QFile file(logfile);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    QString jsonstr= QJsonDocument(joc).toJson();

    stream <<jsonstr.toUtf8().toBase64()<<"\n";
    stream.flush();//强制写出
    file.close();
    // 解锁
    mutex.unlock();
}
void cleanLog()
{
    QStringList fList;
    file_search(logPath,fList);
    for(int i=0;i<fList.size()-3;++i)//保留4个//暂且能按时间来
    {
        qDebug()<<"删除旧日志:"<<fList.at(i);
        QFile(fList.at(i)).remove();
    }
}

int main(int argc, char *argv[])
{
    logPath=getTempPath("temp")
            +updaterTempDir
            +"log/";
    createFolderSlot(logPath);
    logfile=logPath+QString::number(QDateTime::currentDateTime().toUTC().toMSecsSinceEpoch());
    qInstallMessageHandler(MessageOutput);//启动日志
    cleanLog();//清理旧日志


    //qDebug()<<"工作路径"<<QString(plwordPath);
    QString tem="";
    //qDebug()<<argc;
    for(int i = 1; i< argc;++i)
    {
        if(i<2)
        {
            tem =QString::fromLocal8Bit(argv[i]);
        }else{
            tem = tem+" "+QString::fromLocal8Bit(argv[i]);
        }
    }
    if(tem.indexOf("?")>0)//唉
    {
        LPWSTR s=GetCommandLineW();
        LPWSTR *cmdLineAry=CommandLineToArgvW(s,&argc);

        for(int i = 1; i< argc;++i)
        {
            //qDebug()<<QString::fromStdWString(cmdLineAry[i]);
            if(i<2)
            {
                tem =QString::fromStdWString(cmdLineAry[i]);
            }else{
                tem = tem+" "+QString::fromStdWString(cmdLineAry[i]);
            }
        }
    }

    qDebug()<<"传参路径："<<tem;
    QApplication a(argc, argv);
    MainWindow w(NULL,tem);
    w.show();
    return a.exec();
}


